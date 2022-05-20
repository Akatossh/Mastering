// Copyright Epic Games, Inc. All Rights Reserved.

#include "MasteringCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "MasteringInventory.h"
#include "MasteringWeapon.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMasteringCharacter

AMasteringCharacter::AMasteringCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	Inventory = CreateDefaultSubobject<UMasteringInventory>(TEXT("Inventory"));
	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AMasteringCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	if (Inventory != nullptr)
	{
		Inventory->SelectBestWeapon();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMasteringCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMasteringCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMasteringCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMasteringCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMasteringCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMasteringCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMasteringCharacter::OnFire);
	PlayerInputComponent->BindAction("InventoryUp", IE_Pressed, this, &AMasteringCharacter::SelectNextWeapon);
	PlayerInputComponent->BindAction("InventoryDown", IE_Pressed, this, &AMasteringCharacter::SelectPreviousWeapon);
}


void AMasteringCharacter::EquipWeapon(TSubclassOf<class AMasteringWeapon> Weapon)
{
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	if (EquippedWeapon != nullptr)
	{
		World->DestroyActor(EquippedWeapon);
	}

	const FRotator SpawnRotation = GetActorRotation();
	const FVector SpawnLocation = GetActorLocation();
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ActorSpawnParams.Owner = this;

	EquippedWeapon = Cast<AMasteringWeapon>(World->SpawnActor(Weapon, &SpawnLocation, &SpawnRotation, ActorSpawnParams));

	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	}
}

void AMasteringCharacter::OnFire()
{
	if (GetEquippedWeapon() != nullptr)
	{
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		GetEquippedWeapon()->Fire(GetControlRotation(), AnimInstance, Inventory);
	}
}

void AMasteringCharacter::SelectNextWeapon()
{
	Inventory->SelectNextWeapon();
}

void AMasteringCharacter::SelectPreviousWeapon()
{
	Inventory->SelectPreviousWeapon();
}

void AMasteringCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMasteringCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{

}

void AMasteringCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{

}


void AMasteringCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AMasteringCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMasteringCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMasteringCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMasteringCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMasteringCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMasteringCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMasteringCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

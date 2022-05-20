// Fill out your copyright notice in the Description page of Project Settings.


#include "MasteringWeaponPickup.h"
#include "Components/SphereComponent.h"
#include "MasteringCharacter.h"
#include "MasteringInventory.h"

// Sets default values
AMasteringWeaponPickup::AMasteringWeaponPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(30.f);
	SphereCollision->SetupAttachment(WeaponMesh);
}

// Called when the game starts or when spawned
void AMasteringWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AMasteringWeaponPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	AMasteringCharacter* player = Cast<AMasteringCharacter>(OtherActor);
	if (player == nullptr)
	{
		return;
	}

	UMasteringInventory* Inventory = player->GetInventory();
	Inventory->AddWeapon(WeaponClass, Ammunition, WeaponPower);
	Inventory->SelectWeapon(WeaponClass);
	Destroy();
}

// Called every frame
void AMasteringWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator rotationAmount(0.0f, DeltaTime *RotationSpeed, 0.0f);
	AddActorLocalRotation(rotationAmount);
}


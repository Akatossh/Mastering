// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "MasteringWeaponPickup.generated.h"

UCLASS()
class MASTERING_API AMasteringWeaponPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMasteringWeaponPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMasteringWeapon> WeaponClass;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float RotationSpeed = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 Ammunition = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 WeaponPower = -1;


};

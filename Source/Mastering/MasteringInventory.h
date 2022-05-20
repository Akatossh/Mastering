// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MasteringInventory.generated.h"

USTRUCT()
struct FWeaponProperties
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<class AMasteringWeapon> WeaponClass;

	UPROPERTY()
	int WeaponPower;

	UPROPERTY()
	int Ammo;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MASTERING_API UMasteringInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMasteringInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AMasteringWeapon> DefaultWeapon;

	void SelectBestWeapon();

	void SelectWeapon(TSubclassOf<class AMasteringWeapon> Weapon);

	void AddWeapon(TSubclassOf<class AMasteringWeapon> Weapon, uint8 Ammunition, uint8 Power);

	void AddDefaultWeapon();

	FORCEINLINE TSubclassOf<class AMasteringWeapon> GetCurrentWeapon() const { return CurrentWeapon; }

	bool ChangeAmmo(TSubclassOf<class AMasteringWeapon> Weapon, const int ChangeAmmount);

	int FindCurrentWeaponIndex() const;
	void SelectNextWeapon();
	void SelectPreviousWeapon();

protected:
	TArray <FWeaponProperties> WeaponsArray;
	TSubclassOf<class AMasteringWeapon> CurrentWeapon;
	int CurrnetWeaponPower = -1;
	class AMasteringCharacter* MyOwner;
};

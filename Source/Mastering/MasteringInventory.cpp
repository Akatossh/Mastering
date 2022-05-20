// Fill out your copyright notice in the Description page of Project Settings.


#include "MasteringInventory.h"
#include "MasteringCharacter.h"
#include "MasteringWeapon.h"

#define UNLIMITED_AMMO -1

// Sets default values for this component's properties
UMasteringInventory::UMasteringInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	MyOwner = Cast<AMasteringCharacter>(GetOwner());
	check(GetOwner() == nullptr || MyOwner != nullptr);
}


// Called when the game starts
void UMasteringInventory::BeginPlay()
{
	Super::BeginPlay();
	if (DefaultWeapon != nullptr)
	{
		AddWeapon(DefaultWeapon, UNLIMITED_AMMO, 0);
		SelectWeapon(DefaultWeapon);
	}
	
}

void UMasteringInventory::SelectBestWeapon()
{
	for (auto WeaponIt = WeaponsArray.CreateIterator(); WeaponIt; ++WeaponIt)
	{
		//TO DO: Add Weapon seletect cryteria

		{
			//SelectWeapon(*WeaponIt);
			break;
		}
	}
}

void UMasteringInventory::SelectWeapon(TSubclassOf<class AMasteringWeapon> Weapon)
{
	MyOwner->EquipWeapon(Weapon);
	CurrentWeapon = Weapon;
}

void UMasteringInventory::AddWeapon(TSubclassOf<class AMasteringWeapon> Weapon, uint8 Ammunition, uint8 Power)
{
	for (auto WeaponIt = WeaponsArray.CreateIterator(); WeaponIt; ++WeaponIt)
	{
		FWeaponProperties& currentProps = *WeaponIt;
		if (currentProps.WeaponClass == Weapon)
		{
			checkSlow(AmmoCount >= 0);
			currentProps.Ammo += Ammunition;
			return;
		}
	}

	FWeaponProperties weaponProps;
	weaponProps.WeaponClass = Weapon;
	weaponProps.WeaponPower = Power;
	weaponProps.Ammo = Ammunition;
	WeaponsArray.Add(weaponProps);
}

void UMasteringInventory::AddDefaultWeapon()
{

}

bool UMasteringInventory::ChangeAmmo(TSubclassOf<class AMasteringWeapon> Weapon, const int ChangeAmmount)
{
	for (auto WeaponIt = WeaponsArray.CreateIterator(); WeaponIt; ++WeaponIt)
	{
		FWeaponProperties& currentProps = *WeaponIt;
		
		if (currentProps.WeaponClass == Weapon)
		{
			if (currentProps.Ammo == UNLIMITED_AMMO)
				return true;

			
			currentProps.Ammo = FMath::Clamp((currentProps.Ammo + ChangeAmmount), 0, 999);

			if (currentProps.Ammo == 0 && ChangeAmmount < 0)
			{
				return false;
			}
			else
			{
				return true;
			}
				
		}
	}
	return false;
}

int UMasteringInventory::FindCurrentWeaponIndex() const
{
	int currentIndex = 0;
	for (auto WeaponIt = WeaponsArray.CreateConstIterator(); WeaponIt; ++WeaponIt, ++currentIndex)
	{
		if (*WeaponIt->WeaponClass == CurrentWeapon)
			break;
	}

	checkSlow(currentIndex < WeaponsArray.Num());
	return currentIndex;
}

void UMasteringInventory::SelectNextWeapon()
{
	int currentIndex = FindCurrentWeaponIndex();
	
	if (currentIndex == WeaponsArray.Num() - 1)
	{
		SelectWeapon(WeaponsArray[0].WeaponClass);
	}
	else
	{
		SelectWeapon(WeaponsArray[currentIndex + 1].WeaponClass);
	}
}

void UMasteringInventory::SelectPreviousWeapon()
{
	int currentIndex = FindCurrentWeaponIndex();

	if (currentIndex > 0) {
		SelectWeapon(WeaponsArray[currentIndex - 1].WeaponClass);
	}
	else
	{

		SelectWeapon(WeaponsArray[WeaponsArray.Num() - 1].WeaponClass);
	}
}

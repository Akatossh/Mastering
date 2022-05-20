// Fill out your copyright notice in the Description page of Project Settings.

#include "StealthCharacter.h"


void AStealthCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAction("Stealth", IE_Pressed, this, &AStealthCharacter::Stealth);
	PlayerInputComponent->BindAction("Stealth", IE_Released, this, &AStealthCharacter::UnStealth);
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AStealthCharacter::AddControllerPitchInput(float Val)
{
	float scale = bIsStealtched ? PitchYawScale : 1.0f;
	Super::AddControllerPitchInput(Val * scale);
}

void AStealthCharacter::AddControllerYawInput(float Val)
{
	float scale = bIsStealtched ? PitchYawScale : 1.0f;
	Super::AddControllerYawInput(Val * scale);
}

void AStealthCharacter::Stealth()
{
	bIsStealtched = true;
	Super::Crouch();
}

void AStealthCharacter::UnStealth()
{
	bIsStealtched = false;
	Super::UnCrouch();
}
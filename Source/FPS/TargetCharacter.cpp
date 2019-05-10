// Fill out your copyright notice in the Description page of Project Settings.

#include "TargetCharacter.h"

// Sets default values
ATargetCharacter::ATargetCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ATargetCharacter::DamageTarget(float DamageAmount) {
	Health -= DamageAmount;

	if (Health <= 0) {
		bDead = true;
	}
}
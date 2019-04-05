// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
AWeaponBase::AWeaponBase() {
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh"));
	GunMesh->SetupAttachment(RootComponent);
	GunMesh->SetCollisionProfileName(FName(TEXT("BlockAll")));

	Suppressor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Suppressor"));
	Suppressor->SetupAttachment(GunMesh);
	Suppressor->bVisible = false;

	VerticalGrip = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Vertical Grip"));
	VerticalGrip->SetupAttachment(GunMesh);
	VerticalGrip->bVisible = false;

	RedDotSight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Red Dot Sight"));
	RedDotSight->SetupAttachment(GunMesh);
	RedDotSight->bVisible = false;

	S56 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sniper 25x56 Sight"));
	S56->SetupAttachment(GunMesh);
	S56->bVisible = false;

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
	MuzzleLocation->SetupAttachment(GunMesh);

	EmptyShellSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Empty Shell Spawn Location"));
	EmptyShellSpawnLocation->SetupAttachment(GunMesh);
}

void AWeaponBase::SetWeaponSight(EWeaponSightEquipped NewSight) {
	switch (NewSight) {
		case EWeaponSightEquipped::WSE_None: {
			RedDotSight->bVisible = false;
			S56->bVisible = false;
			break;
		}
		case EWeaponSightEquipped::WSE_RDot: {
			RedDotSight->bVisible = true;
			S56->bVisible = false;
			break;
		}
		case EWeaponSightEquipped::WSE_S56: {
			RedDotSight->bVisible = false;
			S56->bVisible = true;
			break;
		}
	}

	Weapon.WeaponSight = NewSight;
}

void AWeaponBase::SetWeaponGrip(EWeaponGripEquipped NewGrip) {
	switch (NewGrip) {
		case EWeaponGripEquipped::WGE_None: {
			VerticalGrip->bVisible = false;
			break;
		}
		case EWeaponGripEquipped::WGE_VGrip: {
			VerticalGrip->bVisible = true;
			break;
		}
	}

	Weapon.WeaponGrip = NewGrip;
}

void AWeaponBase::SetWeaponSuppressor(EWeaponSuppressorEquipped NewSuppressor) {
	switch (NewSuppressor) {
		case EWeaponSuppressorEquipped::WSUE_None: {
			Suppressor->bVisible = false;
			break;
		}
		case EWeaponSuppressorEquipped::WSUE_Standard: {
			Suppressor->bVisible = true;
			break;
		}
	}

	Weapon.WeaponSuppressor = NewSuppressor;
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun Mesh"));
	GunMesh->SetupAttachment(RootComponent);
	GunMesh->SetCollisionProfileName(FName(TEXT("BlockAll")));

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Location"));
	MuzzleLocation->SetupAttachment(GunMesh);

	EmptyShellSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Empty Shell Spawn Location"));
	EmptyShellSpawnLocation->SetupAttachment(GunMesh);
}


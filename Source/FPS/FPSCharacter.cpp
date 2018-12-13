// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"
#include "FPSProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Engine.h"
#include "MainPlayerController.h"
#include "Particles/ParticleSystem.h"
#include "GameplayStructs.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AFPSCharacter

AFPSCharacter::AFPSCharacter()
{
	// Enable Tick
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	Camera->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	PlayerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerMesh"));
	PlayerMesh->SetOnlyOwnerSee(true);
	PlayerMesh->SetupAttachment(Camera);
	PlayerMesh->bCastDynamicShadow = false;
	PlayerMesh->CastShadow = false;
	PlayerMesh->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	PlayerMesh->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	GunMesh->SetupAttachment(Camera);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunMesh);

	EmptyShellSpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Empty Shell Spawn Location"));
	EmptyShellSpawnLocation->SetupAttachment(GunMesh);
}

//////////////////////////////////////////////////////////////////////////
// Input
void AFPSCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::OnFireStart);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSCharacter::OnFireStop);
	// Bind reload event
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSCharacter::OnReloadStart);
	// Bind interact event
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AFPSCharacter::StartInteracting);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}
void AFPSCharacter::OnFireStart()
{
	if (ReloadTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(ReloadTimer);
	}

	if (GetGunMesh()->SkeletalMesh != NULL)
	{
		if (Weapon.CurrentAmmunition > 0 && !bFireTimer)
		{
			bFireTimer = true;
			OnFireProjectile();
		}
		else if (Weapon.CurrentAmmunition > 0)
		{
			UGameplayStatics::PlaySoundAtLocation(this, NoAmmoSound, GetActorLocation());
		}
	}
}
void AFPSCharacter::OnFireStop() 
{
	if (FireTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
		GetWorldTimerManager().SetTimer(StopTimer, this, &AFPSCharacter::StopbFireTimer, Weapon.FireRate, false);
	}
}
void AFPSCharacter::OnFireProjectile()
{
	// try and fire a projectile
	if (Weapon.ProjectileClass != NULL && Weapon.FireSound != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(Weapon.GunOffset);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Weapon.ParticleEffect, FTransform(MuzzleLocation->GetComponentRotation(), MuzzleLocation->GetComponentLocation(), FVector(0.1, 0.1, 0.1)));

		AddControllerPitchInput(Weapon.KickbackAmount * -1);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(Weapon.ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		
		UGameplayStatics::PlaySoundAtLocation(this, Weapon.FireSound, GetActorLocation());

		KnockbackStart();
		Weapon.CurrentAmmunition -= 1;

		if (Weapon.CurrentAmmunition <= 0)
		{
			ReloadAnim(false);
			bFireTimer = false;
		}
		else
		{
			AMainPlayerController* PC = Cast<AMainPlayerController>(GetController());
			TArray<FInputActionKeyMapping> KeyArray = PC->PlayerInput->GetKeysForAction("Fire");

			for (int Index = 0; Index < KeyArray.Num(); Index++)
			{
				if (PC->IsInputKeyDown(KeyArray[Index].Key))
				{
					GetWorldTimerManager().SetTimer(FireTimer, this, &AFPSCharacter::OnFireProjectile, Weapon.FireRate, false);
				}
			}
		}
	}
}
void AFPSCharacter::StopbFireTimer() { bFireTimer = false; }
void AFPSCharacter::OnReloadStart()
{
	if (Weapon.CurrentAmmunition < Weapon.MaxAmmunition) // && Weapon.ReloadAnimation != NULL
	{
		PlayAnimMontage(Weapon.ReloadAnimation);
		GetWorldTimerManager().SetTimer(ReloadTimer, this, &AFPSCharacter::OnReloadEnd, Weapon.ReloadTime, false);
	}
}
void AFPSCharacter::OnReloadEnd()
{
	Weapon.CurrentAmmunition = Weapon.MaxAmmunition;
	ReloadAnim(true);
}
void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}
void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

//////////////////////////////////////////////////////////////////////////
// Tick
void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FHitResult Result;
	FVector Start = Camera->GetComponentLocation();
	FVector ForwardVector = Camera->GetForwardVector();
	FVector End = ((ForwardVector * 1000.f) + Start);
	FCollisionQueryParams CollisionParams(FName(TEXT("Interaction Trace")), true, this);

	if (GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_WorldDynamic, CollisionParams))
	{
		//DrawDebugLine(GetWorld(), Start, End, FColor(255,0,0), false, 0.1, 0, 1);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor(0, 255, 0), Result.GetActor()->GetName(), true);
		WeaponCast = Cast<AWeaponBase>(Result.GetActor());
		
		if (!bInteractShowing && WeaponCast)
		{
			SpawnInteraction();
			bInteractShowing = true;
		}
		else if (bInteractShowing && !WeaponCast)
		{
			RemoveInteraction();
			bInteractShowing = false;
			WeaponCast = NULL;
		}
	}
	else
	{
		if (WeaponCast != NULL)
		{
			WeaponCast = NULL;
		}

		if (bInteractShowing)
		{
			RemoveInteraction();
			bInteractShowing = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Interaction
void AFPSCharacter::StartInteracting()
{
	if (bInteractShowing && WeaponCast)
	{
		if (WeaponCast->Weapon.CurrentAmmunition <= 0)
		{
			ReloadAnim(false);
		}
		else
		{
			ReloadAnim(true);
		}

		if (FireTimer.IsValid())
		{
			GetWorldTimerManager().ClearTimer(FireTimer);
		}
		if (ReloadTimer.IsValid())
		{
			GetWorldTimerManager().ClearTimer(ReloadTimer);
		}

		if (GunMesh->SkeletalMesh != NULL)
		{
			USkeletalMesh* NewGunMesh = WeaponCast->GetGunMesh()->SkeletalMesh;
			FVector NewMuzzleLocation = WeaponCast->GetMuzzleLocation()->RelativeLocation;
			FVector NewEmptyShellSpawnLocation = WeaponCast->GetEmptyShellSpawnLocation()->RelativeLocation;
			FWeaponStruct NewWeapon = WeaponCast->Weapon;
			
			WeaponCast->GetGunMesh()->SetSkeletalMesh(GunMesh->SkeletalMesh);
			WeaponCast->GetMuzzleLocation()->SetRelativeLocation(MuzzleLocation->RelativeLocation);
			WeaponCast->GetEmptyShellSpawnLocation()->SetRelativeLocation(EmptyShellSpawnLocation->RelativeLocation);
			WeaponCast->Weapon = Weapon;

			GunMesh->SetSkeletalMesh(NewGunMesh);
			MuzzleLocation->SetRelativeLocation(NewMuzzleLocation);
			EmptyShellSpawnLocation->SetRelativeLocation(NewEmptyShellSpawnLocation);
			Weapon = NewWeapon;
		}
		else
		{
			GunMesh->SetSkeletalMesh(WeaponCast->GetGunMesh()->SkeletalMesh);
			MuzzleLocation->SetRelativeLocation(WeaponCast->GetMuzzleLocation()->RelativeLocation);
			EmptyShellSpawnLocation->SetRelativeLocation(WeaponCast->GetEmptyShellSpawnLocation()->RelativeLocation);
			Weapon = WeaponCast->Weapon;

			WeaponCast->Destroy(true, true);
		}
	}
}
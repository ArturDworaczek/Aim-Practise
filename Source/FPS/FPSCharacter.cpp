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

	// Create a gun mesh component
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->bCastDynamicShadow = false;
	GunMesh->CastShadow = false;
	GunMesh->SetupAttachment(Camera);

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

	if (GetGunMesh()->SkeletalMesh != nullptr)
	{
		if (Weapon.Weapon.CurrentAmmunition > 0 && !bFireTimer)
		{
			bFireTimer = true;
			OnFireProjectile();
		}
		else if (Weapon.Weapon.CurrentAmmunition > 0)
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
		GetWorldTimerManager().SetTimer(StopTimer, this, &AFPSCharacter::StopbFireTimer, Weapon.Weapon.FireRate, false);
	}
}
void AFPSCharacter::OnFireProjectile()
{
	// try and fire a projectile
	if (Weapon.Weapon.ProjectileClass != nullptr && Weapon.Weapon.FireSound != nullptr)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		FVector SpawnLocation;
		if (!bAiming) {
			SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(Weapon.Weapon.GunOffset);
		}
		else {
			SpawnLocation = ((MuzzleLocation != nullptr) ? MuzzleLocation->GetComponentLocation() : GetActorLocation());
		}

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Weapon.Weapon.ParticleEffect, FTransform(MuzzleLocation->GetComponentRotation(), MuzzleLocation->GetComponentLocation(), FVector(0.1, 0.1, 0.1)));

		AddControllerPitchInput(Weapon.Weapon.KickbackAmount * -1);

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// spawn the projectile at the muzzle
		AFPSProjectile* Projectile = GetWorld()->SpawnActor<AFPSProjectile>(Weapon.Weapon.ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (Projectile) {
			Projectile->ProjectileDamage = Weapon.Weapon.WeaponDamage;
		}
		
		UGameplayStatics::PlaySoundAtLocation(this, Weapon.Weapon.FireSound, GetActorLocation());

		KnockbackStart();
		Weapon.Weapon.CurrentAmmunition -= 1;

		if (Weapon.Weapon.CurrentAmmunition <= 0)
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
					GetWorldTimerManager().SetTimer(FireTimer, this, &AFPSCharacter::OnFireProjectile, Weapon.Weapon.FireRate, false);
				}
			}
		}
	}
}
void AFPSCharacter::StopbFireTimer() { bFireTimer = false; }
void AFPSCharacter::OnReloadStart()
{
	if (Weapon.Weapon.CurrentAmmunition < Weapon.Weapon.MaxAmmunition) // && Weapon.ReloadAnimation != NULL
	{
		PlayAnimMontage(Weapon.Weapon.ReloadAnimation);
		GetWorldTimerManager().SetTimer(ReloadTimer, this, &AFPSCharacter::OnReloadEnd, Weapon.Weapon.ReloadTime, false);
	}
}
void AFPSCharacter::OnReloadEnd()
{
	Weapon.Weapon.CurrentAmmunition = Weapon.Weapon.MaxAmmunition;
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
			WeaponCast = nullptr;
		}
	}
	else
	{
		if (WeaponCast != nullptr)
		{
			WeaponCast = nullptr;
		}

		if (bInteractShowing)
		{
			RemoveInteraction();
			bInteractShowing = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Attachments
void AFPSCharacter::SetWeaponSight(EWeaponSightEquipped NewSight) {
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
void AFPSCharacter::SetWeaponGrip(EWeaponGripEquipped NewGrip) {
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
void AFPSCharacter::SetWeaponSuppressor(EWeaponSuppressorEquipped NewSuppressor) {
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

//////////////////////////////////////////////////////////////////////////
// Interaction
void AFPSCharacter::StartInteracting()
{
	if (bInteractShowing && WeaponCast)
	{
		if (WeaponCast->Weapon.Weapon.CurrentAmmunition <= 0)
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

		if (GunMesh->SkeletalMesh != nullptr)
		{
			USkeletalMesh* NewGunMesh = WeaponCast->GetGunMesh()->SkeletalMesh;
			FVector NewMuzzleLocation = WeaponCast->GetMuzzleLocation()->RelativeLocation;
			FVector NewEmptyShellSpawnLocation = WeaponCast->GetEmptyShellSpawnLocation()->RelativeLocation;
			FWeaponStruct NewWeapon = WeaponCast->Weapon;
			
			WeaponCast->GetGunMesh()->SetSkeletalMesh(GunMesh->SkeletalMesh);
			WeaponCast->GetMuzzleLocation()->SetRelativeLocation(MuzzleLocation->RelativeLocation);
			WeaponCast->GetEmptyShellSpawnLocation()->SetRelativeLocation(EmptyShellSpawnLocation->RelativeLocation);
			WeaponCast->Weapon = Weapon;
			WeaponCast->SetWeaponGrip(Weapon.WeaponGrip);
			WeaponCast->SetWeaponSight(Weapon.WeaponSight);
			WeaponCast->SetWeaponSuppressor(Weapon.WeaponSuppressor);

			GunMesh->SetSkeletalMesh(NewGunMesh);
			MuzzleLocation->SetRelativeLocation(NewMuzzleLocation);
			EmptyShellSpawnLocation->SetRelativeLocation(NewEmptyShellSpawnLocation);
			Weapon = NewWeapon;
			WeaponCast->SetWeaponGrip(NewWeapon.WeaponGrip);
			WeaponCast->SetWeaponSight(NewWeapon.WeaponSight);
			WeaponCast->SetWeaponSuppressor(NewWeapon.WeaponSuppressor);
		}
		else
		{
			GunMesh->SetSkeletalMesh(WeaponCast->GetGunMesh()->SkeletalMesh);
			MuzzleLocation->SetRelativeLocation(WeaponCast->GetMuzzleLocation()->RelativeLocation);
			EmptyShellSpawnLocation->SetRelativeLocation(WeaponCast->GetEmptyShellSpawnLocation()->RelativeLocation);

			S56->SetRelativeLocation(WeaponCast->GetS56()->RelativeLocation);
			RedDotSight->SetRelativeLocation(WeaponCast->GetRedDotSight()->RelativeLocation);
			VerticalGrip->SetRelativeLocation(WeaponCast->GetVerticalGrip()->RelativeLocation);
			Suppressor->SetRelativeLocation(WeaponCast->GetSuppressor()->RelativeLocation);

			Weapon = WeaponCast->Weapon;
			SetWeaponGrip(WeaponCast->Weapon.WeaponGrip);
			SetWeaponSight(WeaponCast->Weapon.WeaponSight);
			SetWeaponSuppressor(WeaponCast->Weapon.WeaponSuppressor);

			WeaponCast->Destroy(true, true);
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSProjectile.h"
#include "Animation/AnimMontage.h"
#include "GameplayStructs.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDetailsStruct
{
	GENERATED_USTRUCT_BODY();

	/** Gun reload animation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) UAnimMontage* ReloadAnimation;
	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) FVector GunOffset;
	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) TSubclassOf<class AFPSProjectile> ProjectileClass;
	/** Particle system to use for the weapon fire. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) UParticleSystem* ParticleEffect;
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) class USoundBase* FireSound;
	/** Maximum Amount Of Ammunition */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) int32 MaxAmmunition;
	/** Current Amount Of Ammunition */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) int32 CurrentAmmunition;
	/** Amount of ammo in stockpile */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) int32 StockPile;
	/** Fire rate to shoot at */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) float FireRate;
	/** Reload time to reload at */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) float ReloadTime;
	/** Kickback amount to kick up after a shot */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) float KickbackAmount;
	/** Weapon damage amount */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) float WeaponDamage;
};

UENUM(BlueprintType)
enum class EWeaponSightEquipped : uint8
{
	WSE_None 	UMETA(DisplayName = "None"),
	WSE_S56 	UMETA(DisplayName = "Scope 25x56"),
	WSE_RDot	UMETA(DisplayName = "Red Dot")
};

UENUM(BlueprintType)
enum class EWeaponGripEquipped : uint8
{
	WGE_None 	UMETA(DisplayName = "None"),
	WGE_VGrip 	UMETA(DisplayName = "Vertical Grip"),
};

UENUM(BlueprintType)
enum class EWeaponSuppressorEquipped : uint8
{
	WSUE_None 		UMETA(DisplayName = "None"),
	WSUE_Standard 	UMETA(DisplayName = "Standard"),
};

USTRUCT(BlueprintType)
struct FWeaponStruct
{
	GENERATED_USTRUCT_BODY();

	/** Weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) FWeaponDetailsStruct Weapon;
	/** Weapon Sight Attached */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) EWeaponSightEquipped WeaponSight;
	/** Weapon Grip Attached */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) EWeaponGripEquipped WeaponGrip;
	/** Weapon Suppressor Attached */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay) EWeaponSuppressorEquipped WeaponSuppressor;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FPS_API UGameplayStructs : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGameplayStructs();
};

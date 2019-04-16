// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayStructs.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "WeaponBase.generated.h"

UCLASS()
class FPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class USkeletalMeshComponent* GunMesh;

	/** Suppressor Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class UStaticMeshComponent* Suppressor;
	/** Vertical Grip Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class UStaticMeshComponent* VerticalGrip;
	/** Gun Red Dot Sight Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class UStaticMeshComponent* RedDotSight;
	/** Gun Sniper 56 Sight Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class UStaticMeshComponent* S56;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class USceneComponent* MuzzleLocation;
	/** Empty shell spawn location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class USceneComponent* EmptyShellSpawnLocation;
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Combat) FWeaponStruct Weapon;

	UFUNCTION(BlueprintCallable, Category = Attachments) 
		void SetWeaponSight(EWeaponSightEquipped NewSight);
	UFUNCTION(BlueprintCallable, Category = Attachments)
		void SetWeaponGrip(EWeaponGripEquipped NewGrip);
	UFUNCTION(BlueprintCallable, Category = Attachments)
		void SetWeaponSuppressor(EWeaponSuppressorEquipped NewSuppressor);
public:
	/** Returns GunMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetGunMesh() const { return GunMesh; }
	/** Returns Suppressor subobject **/
	FORCEINLINE class UStaticMeshComponent* GetSuppressor() const { return Suppressor; }
	/** Returns Vertical Grip subobject **/
	FORCEINLINE class UStaticMeshComponent* GetVerticalGrip() const { return VerticalGrip; }
	/** Returns Red Dot Sight subobject **/
	FORCEINLINE class UStaticMeshComponent* GetRedDotSight() const { return RedDotSight; }
	/** Returns Sniper 56 Sight subobject **/
	FORCEINLINE class UStaticMeshComponent* GetS56() const { return S56; }
	/** Returns MuzzleLocation subobject **/
	FORCEINLINE class USceneComponent* GetMuzzleLocation() const { return MuzzleLocation; }
	/** Returns EmptyShellSpawnLocation subobject **/
	FORCEINLINE class USceneComponent* GetEmptyShellSpawnLocation() const { return EmptyShellSpawnLocation; }
};

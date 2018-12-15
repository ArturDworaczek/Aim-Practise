// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayStructs.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "WeaponBase.generated.h"

UCLASS()
class FPS_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class USkeletalMeshComponent* GunMesh;
	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class USceneComponent* MuzzleLocation;
	/** Empty shell spawn location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class USceneComponent* EmptyShellSpawnLocation;
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	
	UPROPERTY(EditAnywhere, Category = Combat) FWeaponStruct Weapon;
public:
	/** Returns GunMesh subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetGunMesh() const { return GunMesh; }
	/** Returns MuzzleLocation subobject **/
	FORCEINLINE class USceneComponent* GetMuzzleLocation() const { return MuzzleLocation; }
	/** Returns EmptyShellSpawnLocation subobject **/
	FORCEINLINE class USceneComponent* GetEmptyShellSpawnLocation() const { return EmptyShellSpawnLocation; }
};

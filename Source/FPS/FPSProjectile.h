// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Classes/Sound/SoundWave.h"
#include "FPSProjectile.generated.h"

UCLASS(config=Game)
class AFPSProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile) class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true")) class UProjectileMovementComponent* ProjectileMovement;

	/** Projectile decal material after hitting */
	UPROPERTY() UMaterialInterface* BulletHoleMaterial;
	UPROPERTY() UMaterialInterface* BloodSplatterMaterial;

	/** Sound cue to play after hitting a character */
	UPROPERTY() USoundWave* HitSound;

public:
	AFPSProjectile();

	/** Projectile damage amount */
	UPROPERTY(BlueprintReadOnly, Category = "Projectile Data", meta = (ExposeOnSpawn = "true")) float ProjectileDamage;

protected:
	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};


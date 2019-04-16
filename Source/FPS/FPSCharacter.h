// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "GameplayStructs.h"
#include "FPSCharacter.generated.h"

class UInputComponent;

UCLASS(config = Game)
class AFPSCharacter : public ACharacter
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
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true")) class UCameraComponent* Camera;

public:
	AFPSCharacter();

	FTimerHandle FireTimer, ReloadTimer, StopTimer;

	/** Weapon cast from the line trace */
	UPROPERTY(BlueprintReadOnly, Category = Gameplay) AWeaponBase* WeaponCast;
	/** Current weapon struct */
	UPROPERTY(BlueprintReadOnly, Category = Gameplay) FWeaponStruct Weapon;
	/** No-Ammo Sound Effect */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay) class USoundBase* NoAmmoSound;
	/** Is interaction widget showing */
	UPROPERTY(BlueprintReadOnly, Category = Gameplay) bool bInteractShowing;
	/** Is the fire timer */
	UPROPERTY(BlueprintReadOnly, Category = Gameplay) bool bFireTimer;

	/** Is the player aiming */
	UPROPERTY(BlueprintReadWrite, Category = Gameplay) bool bAiming;

protected:
	/** Called every frame */
	virtual void Tick(float DeltaSeconds) override;

	/** Tries to fire projectile */
	UFUNCTION() 
		void OnFireStart();

	/** Stops Shooting */
	UFUNCTION() 
		void OnFireStop();

	/** Fires a projectile */
	UFUNCTION() 
		void OnFireProjectile();

	/** Sets the fire timer boolean to false */
	UFUNCTION()
		void StopbFireTimer();

	/** Starts reload animation */
	UFUNCTION() 
		void OnReloadStart();
	
	/** Ends reload animation */
	UFUNCTION() 
		void OnReloadEnd();

	/** Handles moving forward/backward */
	UFUNCTION() 
		void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	UFUNCTION() 
		void MoveRight(float Val);

	/** Handles the start of the interaction */
	UFUNCTION() 
		void StartInteracting();

	/** Starts knockback movement on weapon */
	UFUNCTION(BlueprintImplementableEvent) 
		void KnockbackStart();

	/** Stops / Plays the reload animation on HUD */
	UFUNCTION(BlueprintImplementableEvent) 
		void ReloadAnim(bool bStopAnimation);

	/** Spawns the interaction widget on the screen */
	UFUNCTION(BlueprintImplementableEvent) 
		void SpawnInteraction();

	/** Removes the interaction widget on the screen */
	UFUNCTION(BlueprintImplementableEvent) 
		void RemoveInteraction();

	UFUNCTION(BlueprintCallable, Category = Attachments)
		void SetWeaponSight(EWeaponSightEquipped NewSight);
	UFUNCTION(BlueprintCallable, Category = Attachments)
		void SetWeaponGrip(EWeaponGripEquipped NewGrip);
	UFUNCTION(BlueprintCallable, Category = Attachments)
		void SetWeaponSuppressor(EWeaponSuppressorEquipped NewSuppressor);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
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


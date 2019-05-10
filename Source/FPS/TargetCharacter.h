// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TargetCharacter.generated.h"

UCLASS()
class FPS_API ATargetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATargetCharacter();

	/** Target health */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target Information") float Health;
	/** Target bool dead */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Target Information") bool bDead;

	/** Apply damage to target */
	UFUNCTION(BlueprintCallable) void DamageTarget(float DamageAmount);
};

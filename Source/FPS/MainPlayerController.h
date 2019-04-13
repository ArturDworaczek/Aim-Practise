// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Classes/Sound/SoundClass.h"
#include "GameFramework/PlayerInput.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/** Get keys binded for action */
	UFUNCTION(BlueprintCallable, Category = "Keybinding") TArray<FText> GetKeyNamesForAction(FName ActionName);
	/** Set new action key */
	UFUNCTION(BlueprintCallable, Category = "Keybinding") bool SetNewActionKey(FName ActionName, FKey NewKey, FKey OldKey);
	/** Get all action key information */
	UFUNCTION(BlueprintCallable, Category = "Keybinding") bool GetActionKeys();
	/** Set new sound class volume */
	UFUNCTION(BlueprintCallable, Category = "Sound") bool SetNewSoundClassVolume(USoundClass* TargetClass, float NewVolume);

	/** Update action key */
	UFUNCTION(BlueprintImplementableEvent, Category = "Keybinding") void UpdateActionKey(FInputActionKeyMapping ActionKeyInformation);
};

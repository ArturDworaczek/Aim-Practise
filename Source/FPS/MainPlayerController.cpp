// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Engine.h"
#include "EngineUtils.h"

// Get keys binded for action
TArray<FText> AMainPlayerController::GetKeyNamesForAction(FName ActionName)
{
	TArray<FInputActionKeyMapping> KeyArray = PlayerInput->GetKeysForAction(ActionName);
	TArray<FText> KeyNameArray;

	for (int Index = 0; Index < KeyArray.Num(); Index++)
	{
		KeyNameArray.Add(KeyArray[Index].Key.GetDisplayName());
	}

	return KeyNameArray;
}
// Set new action key
bool AMainPlayerController::SetNewActionKey(FName ActionName, FKey NewKey, FKey OldKey)
{
	// Get Input Settings
	UInputSettings* Settings = UInputSettings::StaticClass()->GetDefaultObject<UInputSettings>();
	if (!Settings) { return false; }

	// Remove Previous Binding
	FInputActionKeyMapping OldInput(ActionName, OldKey, false, false, false, false);
	Settings->RemoveActionMapping(OldInput);

	// Use New Binding
	FInputActionKeyMapping NewInput(ActionName, NewKey, false, false, false, false);
	Settings->AddActionMapping(NewInput);

	return true;
}
// Get all action key information
bool AMainPlayerController::GetActionKeys()
{
	// Get Input Settings
	UInputSettings* Settings = UInputSettings::StaticClass()->GetDefaultObject<UInputSettings>();
	if (!Settings) { return false; }

	// Return Action Mappings
	for (int8 ActionKey = 0; ActionKey < Settings->ActionMappings.Num(); ActionKey++)
	{
		UpdateActionKey(Settings->ActionMappings[ActionKey]);
	}

	return true;
}
// Set new sound class volume
bool AMainPlayerController::SetNewSoundClassVolume(USoundClass* TargetClass, float NewVolume)
{
	if (!TargetClass) { return false; }

	TargetClass->Properties.Volume = NewVolume;
	return true;
}
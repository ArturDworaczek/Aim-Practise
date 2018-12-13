// Fill out your copyright notice in the Description page of Project Settings.

#include "MainPlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Engine.h"

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
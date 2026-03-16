// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class BBGFunctionLibrary
{
public:
	static void PrintGameDebugMessage(const AActor* InWorldContextActor, const FString& InString, 
											float InTimeToDisplay = 1.f, FColor InColor = FColor::Cyan);
	static FString GetNetModeString(const AActor* InWorldContextActor);
	
	static FString GetRoleString(const AActor* InActor);
};
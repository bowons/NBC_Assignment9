// Copyright Epic Games, Inc. All Rights Reserved.

#include "NBC_Assignment9.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, NBC_Assignment9, "NBC_Assignment9" );

void BBGFunctionLibrary::PrintGameDebugMessage(const AActor* InWorldContextActor, const FString& InString,
	float InTimeToDisplay, FColor InColor)
{
	if (IsValid(GEngine) == true && IsValid(InWorldContextActor) == true)
	{
		if (InWorldContextActor->GetNetMode() == ENetMode::NM_Client || InWorldContextActor->GetNetMode() == ENetMode::NM_ListenServer)
		{
			GEngine->AddOnScreenDebugMessage(-1, InTimeToDisplay, InColor, InString);    
		} 
		else
		{
			UE_LOG(LogTemp, Log, TEXT("%s"), *InString);
		}
	} 
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GEngine or InWorldContextActor is invalid"));	
	}
}

FString BBGFunctionLibrary::GetNetModeString(const AActor* InWorldContextActor)
{
	FString NetModeString = TEXT("None");
	
	if (IsValid(InWorldContextActor) == true)
	{
		ENetMode NetMode = InWorldContextActor->GetNetMode();
		if (NetMode == ENetMode::NM_Client)
		{
			NetModeString = TEXT("Client");
		} 
		else
		{
			if (NetMode == ENetMode::NM_Standalone)
			{
				NetModeString = TEXT("Standalone");
			} 
			else
			{
				NetModeString = TEXT("Server");
			}
		}
	}
	
	return NetModeString;
}

FString BBGFunctionLibrary::GetRoleString(const AActor* InActor)
{
	FString RoleString = TEXT("None");
	
	if (IsValid(InActor) == true)
	{
		FString LocalRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetLocalRole());
		FString RemoteRoleString = UEnum::GetValueAsString(TEXT("Engine.ENetRole"), InActor->GetRemoteRole());
		
            RoleString = FString::Printf(TEXT("%s / %s"), *LocalRoleString, *RemoteRoleString);
	}
	
	return RoleString;
}


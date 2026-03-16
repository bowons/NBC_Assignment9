// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BBG_ControllerChatComponent.generated.h"

/**
 * 
 */
UCLASS()
class NBC_ASSIGNMENT9_API UBBG_ControllerChatComponent : public UObject
{
	GENERATED_BODY()
	
public:
	void SetChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString) const;	
	
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Client, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);
	
protected:
	FString ChatMessageString;
};

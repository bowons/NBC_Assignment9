// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBG_ControllerChatComponent.generated.h"

class UBBG_MainWidget;
/**
 * 
 */
UCLASS()
class NBC_ASSIGNMENT9_API UBBG_ControllerChatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void Initialize(UBBG_MainWidget* InMainWidget);
	
	void ProcessChatMessageString(const FString& InChatMessageString);
	void PrintChatMessageString(const FString& InChatMessageString) const;	
	void PrintSystemMessage(const FString& InMessage) const;
	
	void ProcessGuessNumberString(const FString& InGuessNumberString);                                                                                                                                                                                           
	
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Client, Reliable)
	void ClientRPCPrintSystemMessage(const FString& InSystemMessage);
	
	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);
	UFUNCTION(Server, Reliable)                                                                                                                                                                                                                                  
	void ServerRPCDoGuessNumber(const FString& InGuessNumberString);   
	
protected:
	FString ChatMessageString;
	
	UPROPERTY()
	TObjectPtr<UBBG_MainWidget> MainWidget;
};

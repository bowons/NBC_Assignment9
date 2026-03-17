// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/BBG_ControllerChatComponent.h"

#include "Game/BBG_GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/BBG_PlayerController.h"
#include "Player/BBG_PlayerState.h"
#include "UI/BBG_MainWidget.h"

void UBBG_ControllerChatComponent::Initialize(UBBG_MainWidget* InMainWidget)
{
	MainWidget = InMainWidget;
}

void UBBG_ControllerChatComponent::ProcessChatMessageString(const FString& InChatMessageString)
{
	ChatMessageString = InChatMessageString;

	const ABBG_PlayerController* PlayerController = Cast<ABBG_PlayerController>(GetOwner());;
	if (IsValid(PlayerController) == true)
	{
		if (PlayerController->IsLocalController() == true)
		{
			if (const ABBG_PlayerState* PlayerState = PlayerController->GetPlayerState<ABBG_PlayerState>())
			{
				const FString CombinedMessageString = PlayerState->GetPlayerInfoString() + TEXT(":") + InChatMessageString;
				ServerRPCPrintChatMessageString(CombinedMessageString);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABBG_ControllerChatComponent::SetChatMessageString() - PlayerController is not valid."));
	}
}

void UBBG_ControllerChatComponent::PrintChatMessageString(const FString& InChatMessageString) const
{
	if (IsValid(MainWidget) == false)
		return;
	
	MainWidget->AddChatMessage(InChatMessageString);
}

void UBBG_ControllerChatComponent::PrintSystemMessage(const FString& InMessage) const
{
	if (IsValid(MainWidget) == false)
		return;
	
	MainWidget->AddSystemMessage(InMessage);	
}

void UBBG_ControllerChatComponent::ClientRPCPrintSystemMessage_Implementation(const FString& InSystemMessage)
{
	PrintSystemMessage(InSystemMessage);
}

// RPC 구현부 문법, Implementation을 넣어줘야 한다.
void UBBG_ControllerChatComponent::ClientRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	PrintChatMessageString(InChatMessageString);
}

void UBBG_ControllerChatComponent::ServerRPCPrintChatMessageString_Implementation(const FString& InChatMessageString)
{
	AGameModeBase* GM = UGameplayStatics::GetGameMode(this);
	
	if (IsValid(GM) == true)
	{
		ABBG_GameModeBase* BBGGM = Cast<ABBG_GameModeBase>(GM);
		if (IsValid(BBGGM) == true)
		{
			BBGGM->PrintChatMessageToAll(Cast<ABBG_PlayerController>(GetOwner()), InChatMessageString);			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UBBG_ControllerChatComponent::ServerRPCPrintChatMessageString_Implementation() - BBGGM is not valid."));
		}
	}
}

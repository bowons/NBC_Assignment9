// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/BBG_ControllerChatComponent.h"

#include "Game/BBG_GameModeBase.h"
#include "Game/BBG_GameStateBase.h"
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

void UBBG_ControllerChatComponent::ProcessGuessNumberString(const FString& InGuessNumberString)
{
	const ABBG_PlayerController* PC = Cast<ABBG_PlayerController>(GetOwner());
	if (IsValid(PC) == false || PC->IsLocalController() == false) return;
	
	if (const ABBG_GameStateBase* GS = GetWorld()->GetGameState<ABBG_GameStateBase>())
	{
		const ABBG_PlayerState* PS = PC->GetPlayerState<ABBG_PlayerState>();                                                                                                                                                                                         
		if (IsValid(PS) == false || PS->PlayerNameString != GS->CurrentTurnPlayerName)                                                                                                                                                                               
		{
			PrintSystemMessage(TEXT("현재 당신의 턴이 아닙니다."));
			return;
		}
	}
	ServerRPCDoGuessNumber(InGuessNumberString);
}

void UBBG_ControllerChatComponent::ServerRPCDoGuessNumber_Implementation(const FString& InGuessNumberString)
{
	ABBG_GameModeBase* GM = Cast<ABBG_GameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(GM))
	{
		GM->DoGuessingNumber(Cast<ABBG_PlayerController>(GetOwner()), InGuessNumberString);
	}
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

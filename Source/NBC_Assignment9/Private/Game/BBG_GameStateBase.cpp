// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Game/BBG_GameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/BBG_PlayerController.h"
#include "Player/Components/BBG_ControllerChatComponent.h"

void ABBG_GameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			ABBG_PlayerController* BBGPlayerController = Cast<ABBG_PlayerController>(PC);
			if (IsValid(BBGPlayerController) == true)
			{
				FString NotificationString = InNameString + TEXT(" 님이 입장하셨습니다.");
				BBGPlayerController->GetControllerChatComponent()->PrintChatMessageString(NotificationString);
			}
		}
	}
}

void ABBG_GameStateBase::MulticastRPCBroadcastSystemMessage_Implementation(const FString& InSystemMessage, float InDuration)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC))
		{
			ABBG_PlayerController* BBGPlayerController = Cast<ABBG_PlayerController>(PC);
			if (IsValid(BBGPlayerController) == true)
			{
				BBGPlayerController->ShowNotification(InSystemMessage, InDuration);
			}
		}
	}
}

void ABBG_GameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, RemainingTime);
	DOREPLIFETIME(ThisClass, CurrentTurnPlayerIndex);
}

void ABBG_GameStateBase::OnRep_RemainingTime()
{
	OnRemainingTimeChanged.Broadcast(RemainingTime);
}

void ABBG_GameStateBase::OnRep_CurrentTurnPlayerIndex()
{
	OnTurnChanged.Broadcast(CurrentTurnPlayerIndex);
}

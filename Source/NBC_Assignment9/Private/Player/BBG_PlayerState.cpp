// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Player/BBG_PlayerState.h"

#include "Net/UnrealNetwork.h"

ABBG_PlayerState::ABBG_PlayerState() : PlayerNameString(TEXT("Player")), CurrentGuessCount(0), MaxGuessCount(0)
{
	// Replicated로 설정된 변수는 반드시 생성자에서 초기화해주는 것을 권장한다.
	bReplicates = true;
}

void ABBG_PlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
}

FString ABBG_PlayerState::GetPlayerInfoString() const
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/")
								+ FString::FromInt(MaxGuessCount) + TEXT(")");
	return PlayerInfoString;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BBG_GameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeChanged, int32, InTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTurnChanged, int32, InTurnIndex);

/**
 * 
 */
UCLASS()
class NBC_ASSIGNMENT9_API ABBG_GameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString = FString((TEXT("XXXXXX"))));
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastSystemMessage(const FString& InSystemMessage, float InDuration = 0.f);
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing = OnRep_RemainingTime)
	int32 RemainingTime;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentTurnPlayerIndex)
	int32 CurrentTurnPlayerIndex;	
	
	FOnRemainingTimeChanged OnRemainingTimeChanged;
	FOnTurnChanged OnTurnChanged;
	
private:
	UFUNCTION()
	void OnRep_RemainingTime();
	
	UFUNCTION()
	void OnRep_CurrentTurnPlayerIndex();
};

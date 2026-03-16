// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BBG_GameStateBase.generated.h"

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
};

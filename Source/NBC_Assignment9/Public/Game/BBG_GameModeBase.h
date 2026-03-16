// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BBG_GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class NBC_ASSIGNMENT9_API ABBG_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	
	void ResetGame();
	
};

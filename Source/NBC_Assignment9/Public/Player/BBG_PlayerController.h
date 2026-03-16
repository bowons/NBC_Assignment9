// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBG_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NBC_ASSIGNMENT9_API ABBG_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABBG_PlayerController();
	
	virtual void BeginPlay() override;
	
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Game/BBG_GameModeBase.h"

#include "Player/BBG_PlayerController.h"

void ABBG_GameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABBG_GameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	ABBG_PlayerController* PlayerController = Cast<ABBG_PlayerController>(NewPlayer);
	if (IsValid(PlayerController) == true)
	{
			
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::OnPostLogin() - PlayerController is not valid."));
	}
}

void ABBG_GameModeBase::ResetGame()
{
}

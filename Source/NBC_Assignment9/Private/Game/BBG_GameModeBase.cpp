// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Game/BBG_GameModeBase.h"

void ABBG_GameModeBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABBG_GameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
}

void ABBG_GameModeBase::ResetGame()
{
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Player/BBG_DefaultPawn.h"

#include "NBC_Assignment9/NBC_Assignment9.h"

// Called when the game starts or when spawned
void ABBG_DefaultPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = BBGFunctionLibrary::GetRoleString(this);
	// TODO : 여기서는 로그만 찍는다, 실제 게임에는 TEXTBOX를 이용해 채팅을 주고받으면서 게임을 하도록 함
	// 이 함수는 Network의 동작을 이해하기 위해 작성되었다.
}

void ABBG_DefaultPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	// TODO : 여기서는 로그만 찍는다, 실제 게임에는 TEXTBOX를 이용해 채팅을 주고받으면서 게임을 하도록 함
	// 이 함수는 Network의 동작을 이해하기 위해 작성되었다.
}
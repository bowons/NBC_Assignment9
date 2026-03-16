// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Player/BBG_PlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Player/Components/BBG_ControllerChatComponent.h"
#include "UI/BBG_MainWidget.h"

ABBG_PlayerController::ABBG_PlayerController() : NotificationText(FText::FromString(TEXT("Notification")))
{
	bReplicates = true;
	ControllerChatComponent = CreateDefaultSubobject<UBBG_ControllerChatComponent>(TEXT("ControllerChatComponent"));
}

void ABBG_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() == false)
	{
		return;
	}
	
	if (IsValid(GameMainWidgetClass) == true)
	{
		GameMainWidgetInstance = CreateWidget<UBBG_MainWidget>(this, GameMainWidgetClass);
		if (IsValid(GameMainWidgetClass))
		{
			GameMainWidgetInstance->AddToViewport();
			FInputModeUIOnly InputModeUIOnly;
			SetInputMode(InputModeUIOnly);
			InputModeUIOnly.SetWidgetToFocus(GameMainWidgetInstance->TakeWidget());
			bShowMouseCursor = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ABBG_PlayerController::BeginPlay() - GameMainWidgetInstance is not valid."));
		}
	}
	
}

void ABBG_PlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, NotificationText);
}

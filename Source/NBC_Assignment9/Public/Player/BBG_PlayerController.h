// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBG_PlayerController.generated.h"

class UBBG_ControllerChatComponent;
/**
 * 
 */
class UBBG_MainWidget;

UCLASS()
class NBC_ASSIGNMENT9_API ABBG_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABBG_PlayerController();
	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBBG_MainWidget> GameMainWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UBBG_MainWidget> GameMainWidgetInstance;
	
public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat")
	TObjectPtr<UBBG_ControllerChatComponent> ControllerChatComponent;
};

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
	
	//Getter
	UBBG_ControllerChatComponent* GetControllerChatComponent() const { return ControllerChatComponent; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBBG_MainWidget> GameMainWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UBBG_MainWidget> GameMainWidgetInstance;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chat")
	TObjectPtr<UBBG_ControllerChatComponent> ControllerChatComponent;

public:
	void ShowNotification(const FString& InMessage, float InDuration = 0.f) const;
	
};

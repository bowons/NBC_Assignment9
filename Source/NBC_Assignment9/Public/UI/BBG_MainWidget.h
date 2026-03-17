// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BBG_MainWidget.generated.h"

/**
 * 
 */

class UEditableTextBox;
class UTextBlock;
class UScrollBox;

UCLASS()
class NBC_ASSIGNMENT9_API UBBG_MainWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatTextScrollPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> InputTextBox;
	
	FTimerHandle NotificationTimerHandle;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	UFUNCTION()
	void OnChatInputTextCommited(const FText& Text, ETextCommit::Type CommitMethod) const;
	
public:
	void AddChatMessage(const FString& InMessage) const;
	void AddSystemMessage(const FString& InMessage) const;
	void SetNotificationMessage(const FText& InMessage, const float InTimerDuration = 0.f);	
};

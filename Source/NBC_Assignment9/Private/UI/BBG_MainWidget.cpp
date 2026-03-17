// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BBG_MainWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Player/BBG_PlayerController.h"
#include "Player/Components/BBG_ControllerChatComponent.h"

void UBBG_MainWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBBG_MainWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UBBG_MainWidget::OnChatInputTextCommited(const FText& Text, const ETextCommit::Type CommitMethod) const
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ABBG_PlayerController* OwningPlayerController = GetOwningLocalPlayer<ABBG_PlayerController>();
		if (IsValid(OwningPlayerController) == true)
		{
			OwningPlayerController->GetControllerChatComponent()->ProcessChatMessageString(Text.ToString());
			InputTextBox->SetText(FText());
		}
	}
}

void UBBG_MainWidget::AddChatMessage(const FString& InMessage) const
{
	if (IsValid(ChatTextScrollPanel) == false)
	{	
		UE_LOG(LogTemp, Error, TEXT("ChatTextScrollPanel is invalid"));
		return;
	}

	constexpr int32 MaxMessage = 50;
	while (ChatTextScrollPanel->GetChildrenCount() >= MaxMessage)
	{
		ChatTextScrollPanel->RemoveChildAt(0);
	}
	
	UTextBlock* NewTextBlock = NewObject<UTextBlock>();
	NewTextBlock->SetText(FText::FromString(InMessage));
	NewTextBlock->SetAutoWrapText(true);
	
	ChatTextScrollPanel->AddChild(NewTextBlock);
	ChatTextScrollPanel->ScrollToEnd(); // 마지막 줄로 스크롤 하는 함수
}

void UBBG_MainWidget::AddSystemMessage(const FString& InMessage) const
{
	if (IsValid(ChatTextScrollPanel) == false)
	{	
		UE_LOG(LogTemp, Error, TEXT("ChatTextScrollPanel is invalid"));
		return;
	}

	constexpr int32 MaxMessage = 50;
	while (ChatTextScrollPanel->GetChildrenCount() >= MaxMessage)
	{
		ChatTextScrollPanel->RemoveChildAt(0);
	}
	
	UTextBlock* NewTextBlock = NewObject<UTextBlock>();
	NewTextBlock->SetText(FText::FromString(FString::Printf(TEXT("[시스템] %s"), *InMessage)));
	NewTextBlock->SetAutoWrapText(true);
	NewTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	
	ChatTextScrollPanel->AddChild(NewTextBlock);
	ChatTextScrollPanel->ScrollToEnd(); // 마지막 줄로 스크롤 하는 함수
}

void UBBG_MainWidget::SetNotificationMessage(const FText& InMessage, const float InTimerDuration)
{
	if (IsValid(NotificationText) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("NotificationText is invalid"));
		return;
	}
	
	NotificationText->SetText(InMessage);
	
	if (FMath::IsNearlyZero(InTimerDuration) == false)
	{
		GetWorld()->GetTimerManager().SetTimer(
			NotificationTimerHandle,
			[this]()
			{
				NotificationText->SetText(FText::GetEmpty());
				GetWorld()->GetTimerManager().ClearTimer(NotificationTimerHandle);
			},
			InTimerDuration,
			false
		);
	}
}

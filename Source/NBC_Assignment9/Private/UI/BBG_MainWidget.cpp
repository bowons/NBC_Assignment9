// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BBG_MainWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Game/BBG_GameStateBase.h"
#include "Player/BBG_PlayerController.h"
#include "Player/BBG_PlayerState.h"
#include "Player/Components/BBG_ControllerChatComponent.h"

void UBBG_MainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(InputTextBox))
	{
		InputTextBox->OnTextCommitted.AddDynamic(this, &UBBG_MainWidget::OnChatInputTextCommited);
	}

	if (ABBG_GameStateBase* GameStateBase = GetWorld()->GetGameState<ABBG_GameStateBase>())
	{
		GameStateBase->OnRemainingTimeChanged.AddDynamic(this, &UBBG_MainWidget::UpdateTimerText);
		//GameStateBase->OnTurnChanged.AddDynamic(this, &UBBG_MainWidget::UpdateTurnInfo);
		GameStateBase->OnTurnChangedByName.AddDynamic(this, &UBBG_MainWidget::UpdateTurnInfoByName);
	}
}

void UBBG_MainWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (IsValid(InputTextBox))
	{
		InputTextBox->OnTextCommitted.RemoveDynamic(this, &UBBG_MainWidget::OnChatInputTextCommited);
	}

	if (ABBG_GameStateBase* GameStateBase = GetWorld()->GetGameState<ABBG_GameStateBase>())
	{
		GameStateBase->OnRemainingTimeChanged.RemoveDynamic(this, &UBBG_MainWidget::UpdateTimerText);
		//GameStateBase->OnTurnChanged.RemoveDynamic(this, &UBBG_MainWidget::UpdateTurnInfo);
		GameStateBase->OnTurnChangedByName.RemoveDynamic(this, &UBBG_MainWidget::UpdateTurnInfoByName);
	}
}

FReply UBBG_MainWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		bIsGuessMode = !bIsGuessMode;
		const FString ModeString = bIsGuessMode ? TEXT("답안 제출 모드 전환") : TEXT("채팅 모드 전환");
		AddSystemMessage(ModeString);
		return FReply::Handled(); // Tab InputBox 전달되는 것 차단함
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UBBG_MainWidget::OnChatInputTextCommited(const FText& Text, const ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		ABBG_PlayerController* OwningPlayerController = GetOwningPlayer<ABBG_PlayerController>();
		if (IsValid(OwningPlayerController) == true)
		{
			if (bIsGuessMode)
			{
				OwningPlayerController->GetControllerChatComponent()->ProcessGuessNumberString(Text.ToString());
			}
			else
			{
				OwningPlayerController->GetControllerChatComponent()->ProcessChatMessageString(Text.ToString());
			}

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

void UBBG_MainWidget::UpdateTimerText(int32 InTime)
{
	int32 Minutes = InTime / 60;
	int32 Seconds = InTime % 60;

	FString PlayerName;
	if (ABBG_GameStateBase* GS = GetWorld()->GetGameState<ABBG_GameStateBase>())
	{
		PlayerName = GS->CurrentTurnPlayerName;
		//int32 TurnIndex = GS->CurrentTurnPlayerIndex;

		// if (GS->PlayerArray.IsValidIndex(TurnIndex))                                                                                                                                                                                                         
		// {       
		//     if (ABBG_PlayerState* PS = Cast<ABBG_PlayerState>(GS->PlayerArray[TurnIndex]))                                                                                                                                                                   
		//         PlayerName = PS->PlayerNameString;                                                                                                                                                                                                           
		// }                                                                                                                                                                                                                                                    
	}

	FText DisplayText = FText::FromString(
		FString::Printf(TEXT("%s | 남은 시간: %02d:%02d"), *PlayerName, Minutes, Seconds));

	TimerText->SetText(DisplayText);
}

void UBBG_MainWidget::UpdateTurnInfo(int32 InTurnIndex)
{
	ABBG_GameStateBase* GameStateBase = GetWorld()->GetGameState<ABBG_GameStateBase>();
	if (IsValid(GameStateBase) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("GameStateBase is invalid"));
		return;
	}

	TArray<TObjectPtr<APlayerState>> PArray = GameStateBase->PlayerArray;

	ABBG_PlayerState* BBGPS = Cast<ABBG_PlayerState>(PArray[InTurnIndex]);
	if (IsValid(BBGPS))
	{
		const FString TurnInfoString = FString::Printf(TEXT("현재 턴: %s"), *BBGPS->PlayerNameString);
		AddSystemMessage(TurnInfoString);
	}
}

void UBBG_MainWidget::UpdateTurnInfoByName(FString InTurnPlayerName)
{
	AddSystemMessage(FString::Printf(TEXT("현재 턴: %s"), *InTurnPlayerName));                                                                                                                                                                               
}

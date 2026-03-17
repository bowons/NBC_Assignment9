// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Game/BBG_GameModeBase.h"

#include "Game/BBG_GameStateBase.h"
#include "Player/BBG_PlayerController.h"
#include "Player/BBG_PlayerState.h"
#include "Player/Components/BBG_ControllerChatComponent.h"
#include "Windows/WindowsApplication.h"

void ABBG_GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void ABBG_GameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABBG_PlayerController* PlayerController = Cast<ABBG_PlayerController>(NewPlayer);
	if (IsValid(PlayerController) == true)
	{
		//PlayerController->NotificationText = FText::FromString(TEXT("서버에 연결되었습니다."));
		AllPlayerControllers.Add(PlayerController);

		ABBG_PlayerState* BBGPS = PlayerController->GetPlayerState<ABBG_PlayerState>();
		if (IsValid(BBGPS) == true)
		{
			BBGPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
			BBGPS->MaxGuessCount = this->MaxGuessCount; // 최대 3회 추측 가능
		}

		ABBG_GameStateBase* BBGGameStateBase = GetGameState<ABBG_GameStateBase>();
		if (IsValid(BBGGameStateBase) == true)
		{
			BBGGameStateBase->MulticastRPCBroadcastLoginMessage(BBGPS->PlayerNameString);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::OnPostLogin() - PlayerController is not valid."));
	}
}

void ABBG_GameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ABBG_PlayerController* BBGPC = Cast<ABBG_PlayerController>(Exiting);
	if (IsValid(BBGPC) == true)
	{
		AllPlayerControllers.Remove(BBGPC);
	}
}

FString ABBG_GameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;

	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](const int32 Num) -> bool { return Num > 0; });

	FString Result;
	for (int32 i = 0; i < 3; i++)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABBG_GameModeBase::IsGuessingNumberCorrect(const FString& InNumberString)
{
	bool bCanPlay = false;

	do
	{
		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;
	}
	while (false);

	return bCanPlay;
}

void ABBG_GameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& BBGPC : AllPlayerControllers)
	{
		ABBG_PlayerState* BBGPS = BBGPC->GetPlayerState<ABBG_PlayerState>();
		if (IsValid(BBGPS) == true)
		{
			BBGPS->CurrentGuessCount = 0;
		}
	}
}

FString ABBG_GameModeBase::JudgeGuessNumber(const FString& InGuessString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (SecretNumberString[i] == InGuessString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessString[i]);
			if (SecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABBG_GameModeBase::JudgeGame(const ABBG_PlayerController* InChattingPlayerController, int32 StrikeCount)
{
	if (3 == StrikeCount)
	{
		ABBG_PlayerState* BBGPS = InChattingPlayerController->GetPlayerState<ABBG_PlayerState>();
		if (IsValid(BBGPS))
		{
			FString WinMessage = BBGPS->PlayerNameString + TEXT(" 님이 승리하였습니다.");

			ABBG_GameStateBase* GS = GetGameState<ABBG_GameStateBase>();
			if (IsValid(GS))
			{
				GS->MulticastRPCBroadcastSystemMessage(WinMessage, 0.f); // 전체 공지
			}
		}
		ResetGame();
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BBGPC : AllPlayerControllers)
		{
			ABBG_PlayerState* BBGPS = BBGPC->GetPlayerState<ABBG_PlayerState>();
			if (IsValid(BBGPS) == true)
			{
				if (BBGPS->CurrentGuessCount < BBGPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::JudgeGame() - BBGPS is not valid."));
				return;
			}
		}

		if (bIsDraw)
		{
			ABBG_GameStateBase* GS = GetGameState<ABBG_GameStateBase>();
			if (IsValid(GS))
			{
				GS->MulticastRPCBroadcastSystemMessage(TEXT("무승부로 게임이 끝났습니다."));
			}
			ResetGame();
		}
	}
}

void ABBG_GameModeBase::PrintChatMessageToAll(ABBG_PlayerController* InChattingPlayerController,
                                              const FString& InChatMessageString)
{
	for (TObjectPtr<ABBG_PlayerController> PlayerController : AllPlayerControllers)
	{
		if (IsValid(PlayerController) == true)
		{
			UBBG_ControllerChatComponent* ChatComponent = PlayerController->GetControllerChatComponent();
			// 여기서는 ProcessChat이 아니라 하나하나 ClientRPC로 채팅을 생성해주어야 한다. 

			ABBG_PlayerState* BBGPS = PlayerController->GetPlayerState<ABBG_PlayerState>();
			if (IsValid(BBGPS) == false)
			{
				UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::PrintChatMessageToAll() - BBGPS is not valid."));
				return;
			}

			//const FString CombinedMessageString = BBGPS->GetPlayerInfoString() + TEXT(":") + InChatMessageString;
			ChatComponent->ClientRPCPrintChatMessageString(InChatMessageString);
		}
	}
}

void ABBG_GameModeBase::DoGuessingNumber(const ABBG_PlayerController* InGuessingPlayerController,
                                         const FString& InGuessNumberString)
{
	FString GuessNumberString = InGuessNumberString;
	if (IsGuessingNumberCorrect(GuessNumberString) == true)
	{
		FString JudgeResultString = JudgeGuessNumber(GuessNumberString);
		IncreaseGuessCount(InGuessingPlayerController);;
	
		const ABBG_PlayerState* BBGPS = InGuessingPlayerController->GetPlayerState<ABBG_PlayerState>();
		if (IsValid(BBGPS) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::DoGuessingNumber() - BBGPS is not valid."))
			return;
		}

		const FString CombinedMessageString = BBGPS->GetPlayerInfoString() + TEXT(":") + JudgeResultString;
		
		for (TObjectPtr<ABBG_PlayerController> BBGPC : AllPlayerControllers)
		{
			if (IsValid(BBGPC) == true)
			{
				BBGPC->GetControllerChatComponent()->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}
		
		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InGuessingPlayerController, StrikeCount);
	}
}

void ABBG_GameModeBase::IncreaseGuessCount(const ABBG_PlayerController* InChattingPlayerController)
{
	ABBG_PlayerState* BBGPS = InChattingPlayerController->GetPlayerState<ABBG_PlayerState>();
	if (IsValid(BBGPS) == true)
	{
		BBGPS->CurrentGuessCount++;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::IncreaseGuessCount() - BBGPS is not valid."));
	}
}

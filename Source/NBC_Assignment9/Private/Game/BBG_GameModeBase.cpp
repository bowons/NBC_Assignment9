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
	
	ABBG_GameStateBase* BBGGS = GetGameState<ABBG_GameStateBase>();
	BBGGS->CurrentTurnPlayerIndex = 0;
	
	StartTurn(BBGGS->CurrentTurnPlayerIndex);
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
			
			if (AllPlayerControllers.Num() - 1 == BBGGameStateBase->CurrentTurnPlayerIndex)                                                                                                                                                                              
				BBGGameStateBase->CurrentTurnPlayerName = BBGPS->PlayerNameString;
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

bool ABBG_GameModeBase::IsGuessingNumberCorrect(const FString& InNumberString, FString& OutErrorMessage)
{
	bool bCanPlay = false;

	do
	{
		if (InNumberString.Len() != 3)
		{
			OutErrorMessage = TEXT("3자리 숫자를 입력하세요.");
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				OutErrorMessage = TEXT("1부터 9까지의 숫자만 입력하세요.");
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}
		
		if (UniqueDigits.Num() != 3)
		{
			OutErrorMessage = TEXT("중복되지 않은 숫자를 입력해주세요.");
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
			BBGPS->bHasPlayedThisTurn = false;
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

bool ABBG_GameModeBase::JudgeGame(const ABBG_PlayerController* InChattingPlayerController, int32 StrikeCount)
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
		return true;
	}
	return CheckAndHandleDraw();
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

void ABBG_GameModeBase::DoGuessingNumber(ABBG_PlayerController* InGuessingPlayerController,
                                         const FString& InGuessNumberString)
{
	ABBG_GameStateBase* GameStateBase = GetGameState<ABBG_GameStateBase>();
	if (IsValid(GameStateBase))
	{
		int32 TurnIndex = GameStateBase->CurrentTurnPlayerIndex;
		if (AllPlayerControllers.IsValidIndex(TurnIndex) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::DoGuessingNumber() - TurnIndex is out of range."));			
			return;
		}
		
		if (AllPlayerControllers[TurnIndex] != InGuessingPlayerController)
		{
			InGuessingPlayerController->GetControllerChatComponent()
				->ClientRPCPrintSystemMessage(TEXT("현재 당신의 턴이 아닙니다."));
			return;
		}
	}
	
	ABBG_PlayerState* BBGPS = InGuessingPlayerController->GetPlayerState<ABBG_PlayerState>();
	if (IsValid(BBGPS) && BBGPS->CurrentGuessCount >= BBGPS->MaxGuessCount)
	{
		InGuessingPlayerController->GetControllerChatComponent()->ClientRPCPrintSystemMessage(TEXT("기회를 모두 소진하였습니다."));
		return;
	}
	
	FString ErrorMessage;
	if (IsGuessingNumberCorrect(InGuessNumberString, ErrorMessage) == false)
	{
		InGuessingPlayerController->GetControllerChatComponent()->ClientRPCPrintSystemMessage(ErrorMessage);
		return;
	}
	
	FString JudgeResultString = JudgeGuessNumber(InGuessNumberString);
	IncreaseGuessCount(InGuessingPlayerController);
	BBGPS->bHasPlayedThisTurn = true;
	
	const FString CombinedMessageString = BBGPS->GetPlayerInfoString() + TEXT(" Guess:") + JudgeResultString;
	const FString GuessCountString = BBGPS->GetGuessCountString(); // 미리 저장
	
    for (TObjectPtr<ABBG_PlayerController> BBGPC : AllPlayerControllers)
    {
	    if (IsValid(BBGPC))
	    	BBGPC->GetControllerChatComponent()->ClientRPCPrintSystemMessage(CombinedMessageString);
    }
	
	int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
	bool bGameEnded = JudgeGame(InGuessingPlayerController, StrikeCount);
	
	InGuessingPlayerController->GetControllerChatComponent()
		->ClientRPCPrintSystemMessage(GuessCountString);
	
	if (bGameEnded)
	{
		StartTurn(0);
	} 
	else if (BBGPS->CurrentGuessCount >= BBGPS->MaxGuessCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
		EndTurn(GameStateBase->CurrentTurnPlayerIndex);
	}
}

void ABBG_GameModeBase::StartTurn(int32 PlayerIndex)
{
	// 30초로 Remaining Time 설정
	ABBG_GameStateBase* GameStateBase = GetGameState<ABBG_GameStateBase>();
	if (IsValid(GameStateBase) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ABBG_GameModeBase::StartTurn() - GameStateBase is not valid."));
		return;
	}
	
	GameStateBase->RemainingTime = 30;
	GameStateBase->CurrentTurnPlayerIndex = PlayerIndex;
	
	if (AllPlayerControllers.IsValidIndex(PlayerIndex))                                                                                                                                                                                                          
	{                                                                                                                                                                                                                                                          
		ABBG_PlayerState* PS = AllPlayerControllers[PlayerIndex]->GetPlayerState<ABBG_PlayerState>();                                                                                                                                                            
		if (IsValid(PS))                                                                                                                                                                                                                                       
			GameStateBase->CurrentTurnPlayerName = PS->PlayerNameString;                                                                                                                                                                                         
	} 
	
	// Timer로 시간 등록
	GetWorld()->GetTimerManager().SetTimer( TurnTimerHandle, this, &ABBG_GameModeBase::OnTurnTimerTick, 1.f, true);
}

void ABBG_GameModeBase::OnTurnTimerTick()
{
	ABBG_GameStateBase* GameStateBase = GetGameState<ABBG_GameStateBase>();
	GameStateBase->RemainingTime--;
	
	if (GameStateBase->RemainingTime <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(TurnTimerHandle);
		EndTurn(GameStateBase->CurrentTurnPlayerIndex);
	}
}

void ABBG_GameModeBase::EndTurn(int32 PlayerIndex)
{
	ABBG_GameStateBase* GameStateBase = GetGameState<ABBG_GameStateBase>();
	if (IsValid(GameStateBase) == false) return;

	if (AllPlayerControllers.IsValidIndex(PlayerIndex) == false) return;
	
	ABBG_PlayerController* CurrentPC = AllPlayerControllers[PlayerIndex];
	ABBG_PlayerState* BBGPS = CurrentPC->GetPlayerState<ABBG_PlayerState>();
	
	if (IsValid(BBGPS))
	{
		if (BBGPS->bHasPlayedThisTurn == false)
		{
			IncreaseGuessCount(CurrentPC);
			CurrentPC->GetControllerChatComponent()
				->ClientRPCPrintSystemMessage(TEXT("시간 초과로 기회가 소진되었습니다."));
		}
		BBGPS->bHasPlayedThisTurn = false;
	}
	
	if (CheckAndHandleDraw())
	{
		StartTurn(0);
		return;
	}

	int32 NextIndex = (GameStateBase->CurrentTurnPlayerIndex + 1) % AllPlayerControllers.Num();
	StartTurn(NextIndex);
}

bool ABBG_GameModeBase::CheckAndHandleDraw()
{
	for  (const auto& BBGPC : AllPlayerControllers)
	{
		ABBG_PlayerState* BBGPS = BBGPC->GetPlayerState<ABBG_PlayerState>();
		if (IsValid(BBGPS) == false)
		{
			return false;
		}
		
		if (BBGPS->CurrentGuessCount < BBGPS->MaxGuessCount)
		{
			return false; // 기회가 남은 플레이어가 존재
		}
	}
	
	ABBG_GameStateBase* GS = GetGameState<ABBG_GameStateBase>();
	if (IsValid(GS))
	{
		GS->MulticastRPCBroadcastSystemMessage(TEXT("무승부로 게임이 끝났습니다."));
	}
	ResetGame();
	return true;
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

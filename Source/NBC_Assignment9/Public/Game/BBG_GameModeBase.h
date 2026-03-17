// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BBG_GameModeBase.generated.h"

class ABBG_PlayerController;
/**
 * 
 */
UCLASS()
class NBC_ASSIGNMENT9_API ABBG_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
	static FString GenerateSecretNumber();
	static bool IsGuessingNumberCorrect(const FString& InNumberString);
	static void IncreaseGuessCount(const ABBG_PlayerController* InChattingPlayerController);
	
	void ResetGame();
	
	FString JudgeGuessNumber(const FString& InGuessString);
	void JudgeGame(const ABBG_PlayerController* InChattingPlayerController, int32 StrikeCount);
	
	void PrintChatMessageToAll(ABBG_PlayerController* InChattingPlayerController, const FString& InChatMessageString);
	void DoGuessingNumber(const ABBG_PlayerController* InGuessingPlayerController, const FString& InGuessNumberString);
	
protected:
	FString SecretNumberString;
	TArray<TObjectPtr<ABBG_PlayerController>> AllPlayerControllers;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game")
	int32 MaxGuessCount = 3;
	
};

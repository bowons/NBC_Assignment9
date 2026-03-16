// Fill out your copyright notice in the Description page of Project Settings.


#include "NBC_Assignment9/Public/Player/BBG_DefaultPawn.h"


// Sets default values
ABBG_DefaultPawn::ABBG_DefaultPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABBG_DefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABBG_DefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABBG_DefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


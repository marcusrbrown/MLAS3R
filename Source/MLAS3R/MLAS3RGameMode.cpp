// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "MLAS3RGameMode.h"

AMLAS3RGameMode::AMLAS3RGameMode()
{
	PendingState = EGameState::None;
}

void AMLAS3RGameMode::BeginPlay()
{
	if(GEngine)
	{
		UGameUserSettings* gameSettings = GEngine->GetGameUserSettings();
		gameSettings->SetScreenResolution(FIntPoint(768, 1024));
		//gameSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		gameSettings->SetVSyncEnabled(true);
		gameSettings->ApplySettings(false);
	}

	auto gridActorIterator = TActorIterator<AMatch3Grid>(GetWorld());
	if (gridActorIterator)
	{
		Match3Grid = *gridActorIterator;
	}

	auto playfieldActorIterator = TActorIterator<APlayfield>(GetWorld());
	if (playfieldActorIterator)
	{
		Playfield = *playfieldActorIterator;
	}

	auto pcActorIterator = TActorIterator<AMLAS3RPlayerController>(GetWorld());
	if (pcActorIterator)
	{
		PlayerController = *pcActorIterator;
	}
	
	Super::BeginPlay();
}

void AMLAS3RGameMode::Tick(float DeltaSeconds)
{
	if (PendingState != EGameState::None && ActiveState != PendingState)
	{
		PreviousState = ActiveState;
		ActiveState = PendingState;
		PendingState = EGameState::None;
		
		OnStateChanged.Broadcast(ActiveState, PreviousState);
	}
	
	Super::Tick(DeltaSeconds);
}

void AMLAS3RGameMode::RequestGameState(EGameState State)
{
	if (State == EGameState::None) return;
	
	PendingState = State;
}



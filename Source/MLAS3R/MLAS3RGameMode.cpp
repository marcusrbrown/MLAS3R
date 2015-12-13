// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "MLAS3RGameMode.h"
#include "MLAS3RPlayerController.h"
#include "Playfield.h"
#include "Match3Grid.h"

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

    // TODO: marcus@HV: These calls ensure that the PlayerController, Playfield, and Match3Grid
    // instances are created at game startup. Remove these after all blueprints have been updated to
    // use the appropriate accessors.
    GetMatch3Grid();
    GetPlayfield();
    GetPlayerController();

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

AMLAS3RPlayerController * AMLAS3RGameMode::GetPlayerController()
{
    if (PlayerController == nullptr)
    {
        auto pcActorIterator = TActorIterator<AMLAS3RPlayerController>(GetWorld());

        if (pcActorIterator)
        {
            PlayerController = *pcActorIterator;
        }
    }

    return PlayerController;
}

APlayfield * AMLAS3RGameMode::GetPlayfield()
{
    if (Playfield == nullptr)
    {
        auto playfieldActorIterator = TActorIterator<APlayfield>(GetWorld());

        if (playfieldActorIterator)
        {
            Playfield = *playfieldActorIterator;
        }
    }

    return Playfield;
}

AMatch3Grid * AMLAS3RGameMode::GetMatch3Grid()
{
    if (Match3Grid == nullptr)
    {
        auto gridActorIterator = TActorIterator<AMatch3Grid>(GetWorld());

        if (gridActorIterator)
        {
            Match3Grid = *gridActorIterator;
        }
    }

    return Match3Grid;
}

void AMLAS3RGameMode::RequestGameState(EGameState State)
{
	if (State == EGameState::None) return;
	
	PendingState = State;
}



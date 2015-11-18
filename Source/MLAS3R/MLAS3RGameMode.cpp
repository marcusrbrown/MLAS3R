// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "MLAS3RGameMode.h"

void AMLAS3RGameMode::BeginPlay()
{
	if(GEngine)
	{
		UGameUserSettings* gameSettings = GEngine->GetGameUserSettings();
		gameSettings->SetScreenResolution(FIntPoint(768, 1024));
		//MyGameSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		gameSettings->SetVSyncEnabled(true);
		gameSettings->ApplySettings(false);
	}

	auto gridActorIterator = TActorIterator<AMatch3Grid>(GetWorld());

	if (gridActorIterator)
	{
		Match3Grid = *gridActorIterator;
	}
}



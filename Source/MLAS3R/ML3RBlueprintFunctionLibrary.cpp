// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "ML3RBlueprintFunctionLibrary.h"
#include "MLAS3RGameMode.h"

AMLAS3RGameMode* UML3RBlueprintFunctionLibrary::GetMLAS3RGameMode(UObject* WorldContextObject)
{
    // Borrow this logic from UGameplayStatics::GetGameMode().
    auto const* world = GEngine->GetWorldFromContextObject(WorldContextObject);

    return world != nullptr ? world->GetAuthGameMode<AMLAS3RGameMode>() : nullptr;
}

AMLAS3RPlayerController* UML3RBlueprintFunctionLibrary::GetMLAS3RPlayerController(UObject* WorldContextObject)
{
    auto* gameMode = GetMLAS3RGameMode(WorldContextObject);

    return gameMode != nullptr ? gameMode->GetPlayerController() : nullptr;
}

APlayfield* UML3RBlueprintFunctionLibrary::GetPlayfield(UObject * WorldContextObject)
{
    auto* gameMode = GetMLAS3RGameMode(WorldContextObject);

    return gameMode != nullptr ? gameMode->GetPlayfield() : nullptr;
}

AMatch3Grid* UML3RBlueprintFunctionLibrary::GetMatch3Grid(UObject * WorldContextObject)
{
    auto* gameMode = GetMLAS3RGameMode(WorldContextObject);

    return gameMode != nullptr ? gameMode->GetMatch3Grid() : nullptr;
}

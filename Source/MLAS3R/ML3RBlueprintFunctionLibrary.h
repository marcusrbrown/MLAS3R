// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "ML3RBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class MLAS3R_API UML3RBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    // NOTE: marcus@HV: The following UFUNCTIONs mimic the ones found in UGameplayStatics, so that they
    // can be used in place of those UFUNCTIONs where appropriate.

    /** Return a reference to the MLAS3R GameMode. */
    UFUNCTION(BlueprintPure, Category = "MLAS3R References", meta = (WorldContext = "WorldContextObject"))
    static class AMLAS3RGameMode* GetMLAS3RGameMode(UObject* WorldContextObject);

    /** Return a reference to the MLAS3R PlayerController. */
    UFUNCTION(BlueprintPure, Category = "MLAS3R References", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
    static class AMLAS3RPlayerController* GetMLAS3RPlayerController(UObject* WorldContextObject);

    /** Return a reference to the MLAS3R Playfield. */
    UFUNCTION(BlueprintPure, Category = "MLAS3R References", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
    static class APlayfield* GetPlayfield(UObject* WorldContextObject);

    /** Return a reference to the MLAS3R Match3Grid. */
    UFUNCTION(BlueprintPure, Category = "MLAS3R References", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
    static class AMatch3Grid* GetMatch3Grid(UObject* WorldContextObject);
};

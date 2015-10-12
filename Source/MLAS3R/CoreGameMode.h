// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "CoreGameMode.generated.h"


/** Reward granted during Match 3. */
// TODO: marcus@HV: This may be a placeholder, or it may be extended. Dunno yet.
USTRUCT()
struct FMatch3Reward
{
	GENERATED_USTRUCT_BODY()

	/** Reward triggers at this interval. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScoreInterval;

	/** Reward grants this much time upon triggering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeAwarded;
};

/**
 * 
 */
UCLASS(Blueprintable)
class MLAS3R_API ACoreGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	/** The speed at which tiles fall into place. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Game")
	float TileFallSpeed;
};

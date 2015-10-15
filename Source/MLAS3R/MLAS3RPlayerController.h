// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Match3GridTile.h"
#include "MLAS3RPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MLAS3R_API AMLAS3RPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    /** The last move made by a player on the Match 3 grid. */
    UPROPERTY(BlueprintReadOnly, Category = "Match 3 Game")
    EMatch3MoveType LastMove;
};

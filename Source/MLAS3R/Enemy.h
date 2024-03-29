// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Enemy.generated.h"

UENUM()
enum class EnemyColor : uint8
{
    Red,
    Green,
    Blue,
    Yellow,
};

/** Return the EnemyColor enumerator value as a string. */
FString GetEnemyColorAsString(EnemyColor color);

/**
 *
 */
UCLASS(Blueprintable)
class MLAS3R_API AEnemy : public AStaticMeshActor
{
	GENERATED_BODY()
		
public:
	AEnemy();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EnemyColor Color;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FVector CaptureLocation;
};

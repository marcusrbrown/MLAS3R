// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Match3/Match3GridTile.h"
#include "Match3EnemyTile.generated.h"

/**
 * Represents a captured enemy. Declared abstract so that only subclasses can be used within a level.
 */
UCLASS(Abstract)
class MLAS3R_API AMatch3EnemyTile : public AMatch3GridTile
{
	GENERATED_BODY()

public:
    AMatch3EnemyTile();

    class AEnemy* GetEnemy() const;
    void SetEnemy(class AEnemy* NewEnemy);

    uint32 bSyncEnemyLocation : 1;

protected:
    virtual void BeginPlay() override;

    virtual void Tick(float DeltaSeconds) override;

    virtual void OnMatched_Implementation(EMatch3MoveType MoveType) override;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match 3 Enemy Tile")
    class AEnemy* Enemy;
};

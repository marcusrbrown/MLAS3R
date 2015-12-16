// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Match3EnemyTile.h"
#include "Enemy.h"

AMatch3EnemyTile::AMatch3EnemyTile()
    : bSyncEnemyLocation(false)
{
    PrimaryActorTick.bCanEverTick = true;
}

AEnemy* AMatch3EnemyTile::GetEnemy() const
{
    return Enemy;
}

void AMatch3EnemyTile::SetEnemy(AEnemy * NewEnemy)
{
    Enemy = NewEnemy;

    Enemy->OnClicked.AddUniqueDynamic(this, &AMatch3GridTile::TilePress_Mouse);
}

void AMatch3EnemyTile::BeginPlay()
{
    Super::BeginPlay();

}

void AMatch3EnemyTile::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (bSyncEnemyLocation && (Enemy != nullptr))
    {
        Enemy->SetActorLocation(GetActorLocation());
        Enemy->SetActorHiddenInGame(false);
    }
}

void AMatch3EnemyTile::OnMatched_Implementation(EMatch3MoveType MoveType)
{
    if (Enemy != nullptr)
    {
        Enemy->OnClicked.RemoveAll(this);
        GetWorld()->DestroyActor(Enemy);
    }

    Super::OnMatched_Implementation(MoveType);
}


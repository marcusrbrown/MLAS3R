// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Enemy.h"

AEnemy::AEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	auto staticMeshComponent = GetStaticMeshComponent();
	staticMeshComponent->SetMobility(EComponentMobility::Movable);
	
	//staticMeshComponent->SetNotifyRigidBodyCollision(true);
	staticMeshComponent->bGenerateOverlapEvents = 1;
	
	
	// This collides
	SetActorEnableCollision(true);
}

FString GetEnemyColorAsString(EnemyColor color)
{
    auto enumObject = FindObject<UEnum>(ANY_PACKAGE, TEXT("EnemyColor"), true);

    if (enumObject == nullptr)
    {
        return FString("Invalid");
    }

    return enumObject->GetEnumName(static_cast<int32>(color));
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "Enemy.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class MLAS3R_API AEnemy : public AStaticMeshActor
{
	GENERATED_BODY()
		
public:
	AEnemy();
};

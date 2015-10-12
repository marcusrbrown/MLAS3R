// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "CoreGameMode.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MLAS3R_API ACoreGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	void BeginPlay() override;
	
	
	
};

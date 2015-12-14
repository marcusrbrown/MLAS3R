// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "SplineActor.h"
#include "SplineTriggerComponent.generated.h"

UCLASS()
class MLAS3R_API USplineTriggerComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	
public:	
	UPROPERTY()
	ASplineActor* Owner;
};

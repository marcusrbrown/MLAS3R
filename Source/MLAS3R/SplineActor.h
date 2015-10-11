// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "SplineActor.generated.h"

UCLASS()
class MLAS3R_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Category = "Spline", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* Spline;
	
public:
	// Sets default values for this actor's properties
	ASplineActor();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "SplineTriggerComponent.h"
#include "SplineActor.h"


// Sets default values
ASplineActor::ASplineActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	RootComponent = Spline;
	
#if WITH_EDITORONLY_DATA
	SplineTriggerComponent = CreateEditorOnlyDefaultSubobject<USplineTriggerComponent>(TEXT("SplineTrigger"));
	SplineTriggerComponent->Owner = this;
#endif
}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
}


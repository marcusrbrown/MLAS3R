// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "SplineActor.generated.h"

class USplineTriggerComponent;

UENUM()
enum class ESplineTriggerAction : uint8
{
	Fire
};

USTRUCT()
struct MLAS3R_API FSplineTriggerNode
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta=(ClampMin=0.0, UIMin=0.0, ClampMax=1.0, UIMax=1.0))
	float Time;
	
	UPROPERTY(EditAnywhere)
	ESplineTriggerAction Action;
	
	UPROPERTY(EditAnywhere)
	bool bActionValue;
};

UCLASS()
class MLAS3R_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Category = "Spline", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USplineComponent* Spline;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spline)
	TArray<FSplineTriggerNode> Triggers;
	
public:
	// Sets default values for this actor's properties
	ASplineActor();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
private:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
	USplineTriggerComponent* SplineTriggerComponent;
#endif
	
};

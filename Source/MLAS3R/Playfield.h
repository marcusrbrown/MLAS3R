// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Playfield.generated.h"

UCLASS(Blueprintable)
class MLAS3R_API APlayfield : public AActor
{
	GENERATED_BODY()
   
public:
   UPROPERTY(Category = "Bounds", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
   UBoxComponent* Bounds;
   
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Types")
   TSubclassOf<AActor> RedEnemy;
   
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Types")
   TSubclassOf<AActor> BlueEnemy;
   
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Types")
   TSubclassOf<AActor> GreenEnemy;
	
public:
	// Sets default values for this actor's properties
	APlayfield();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

   
#if WITH_EDITOR
   virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
#endif
	
	
};

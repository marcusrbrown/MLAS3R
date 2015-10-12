// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Playfield.generated.h"


UENUM()
enum class EPlayfieldEnemyState : uint8
{
	Intro,
	ToFormation,
	Formation,
	ToAttack,
	Attack,
	Dead
};

USTRUCT(BlueprintType)
struct MLAS3R_API FPlayfieldEnemyData
{
	GENERATED_BODY()
	
	UPROPERTY()
	EPlayfieldEnemyState State;
	
	UPROPERTY()
	AActor* Enemy;
	
	UPROPERTY()
	USplineComponent* Spline;
	
	UPROPERTY()
	int32 Duration;
	
	UPROPERTY(EditAnywhere)
	float DeltaTime;
	
	UPROPERTY()
	TArray<float> BulletTimes;
};


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
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnRedEnemy();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnBlueEnemy();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnGreenEnemy();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Levels")
	TArray<UDataTable*> Levels;
	
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
	
private:
	int32 CurrentLevel;
	
	int32 CurrentRow;
	
	float PlayTime;
	
	TArray<FPlayfieldEnemyData> Enemies;
	
	USplineComponent* FindSplineByName(FString name);
};

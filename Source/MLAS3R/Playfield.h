// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "TableRows.h"
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
	FString Type;
	
	UPROPERTY()
	EPlayfieldEnemyState State;
	
	UPROPERTY(EditAnywhere)
	float DeltaTime;
	
	UPROPERTY()
	AActor* Enemy;
	
	UPROPERTY()
	float Speed;
	
	UPROPERTY()
	int32 GridAddress;
	
	UPROPERTY()
	USplineComponent* IntroSpline;
	
	UPROPERTY()
	TArray<float> IntroBullets;
	
	UPROPERTY()
	int32 IntroBulletIndex;
	
	UPROPERTY()
	USplineComponent* AttackSpline;
	
	UPROPERTY()
	TArray<float> AttackBullets;
	
	UPROPERTY()
	int32 AttackBulletIndex;
	
	UPROPERTY()
	float LerpAlpha;
	
	UPROPERTY()
	float LerpDuration;
};

USTRUCT()
struct MLAS3R_API FPlayfieldGridData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector CellExtent;
	
	UPROPERTY()
	int32 Width;
	
	UPROPERTY()
	int32 Height;
	
	UPROPERTY()
	int32 Rows;
	
	UPROPERTY()
	int32 Columns;
	
	UPROPERTY()
	FVector Pivot;
	
	UPROPERTY()
	FVector Offset;
	
	UPROPERTY()
	FVector Padding;
	
	UPROPERTY()
	float LerpAlpha;
	
	UPROPERTY()
	float LerpAlphaDirection;
	
	UPROPERTY()
	float LerpDuration;
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet Types")
	TSubclassOf<AActor> RedBullet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet Types")
	TSubclassOf<AActor> BlueBullet;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bullet Types")
	TSubclassOf<AActor> GreenBullet;
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnRedEnemy();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnBlueEnemy();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnGreenEnemy();
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnEnemyFromTableRow(const FPlayfieldSpawnTableRow& row);
	
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	AActor* SpawnEnemyBulletAtLocation(const FString& type, const FVector& location);
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetPlayerIsDead(bool value);
	
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
	
protected:
	USplineComponent* FindSplineByName(FString name);
	
	void ParseBulletString(const FString& bulletString, TArray<float>& OutArray);
	
	FVector GetGridLocationFromAddress(int32 address);
	
private:
	int32 CurrentLevel;
	
	int32 CurrentRow;
	
	float PlayTime;
	
	float SpeedMultiplier;
	
	bool PlayerIsDead;
	
	TArray<FPlayfieldEnemyData> Enemies;
	
	FPlayfieldGridData Grid;
};

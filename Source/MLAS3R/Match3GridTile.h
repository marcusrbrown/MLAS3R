// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Match3GridTile.generated.h"

class AMatch3Grid;

UENUM()
enum class EMatch3TileState : uint8
{
	Inactive,
	Active,
	Falling,
	Removed,
};

UENUM(BlueprintType)
enum class EMatch3MoveType : uint8
{
	None,
	Invalid,
	Swap,
};

USTRUCT()
struct FTileAbilities
{
	GENERATED_USTRUCT_BODY()

	bool CanSwap() const { return !bPreventSwapping; }

protected:
	/** Tile can't be selected as part of a normal swapping move. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint32 bPreventSwapping : 1;
};

UCLASS(Blueprintable)
class MLAS3R_API AMatch3GridTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatch3GridTile();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
	void TileSelected();

	UFUNCTION()
	void TileSelectedByTouch(ETouchIndex::Type FingerIndex);
	
	/** Called when a match has been made. Reports the EMatch3MoveType that created the match. */
	UFUNCTION(BlueprintNativeEvent, Category = "Match 3 Game Events")
	void OnMatched(EMatch3MoveType MoveType);
	virtual void OnMatched_Implementation(EMatch3MoveType MoveType);
	
	UFUNCTION(BlueprintNativeEvent, Category = "Match 3 Game Events")
	void OnSwapMove(AMatch3GridTile* DestinationTile, bool bMoveWillSucceed);
	virtual void OnSwapMove_Implementation(AMatch3GridTile* DestinationTile, bool bMoveWillSucceed);

	/** Play an effect when the tile is selected. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Match 3 Game Events")
	void PlaySelectionEffect(bool bTurnEffectOn);

	void StartFalling(bool bUseCurrentWorldLocation = false);
	void FinishFalling();
	
	UFUNCTION()
	void TickFalling();
	
	int32 GetGridAddress() const;
	void SetGridAddress(int32 NewAddress);
	
	UPROPERTY()
	int32 TileTypeID;
	
	UPROPERTY()
	EMatch3TileState TileState;

	UPROPERTY()
	FTileAbilities Abilities;
	
protected:
	float FallingStartTime;
	float TotalFallingTime;
	FVector FallingStartLocation;
	FVector FallingEndLocation;
	FTimerHandle TickFallingHandle;

	/** The grid that owns this tile. */
	UPROPERTY(BlueprintReadOnly, Category = "Match 3 Tile")
	AMatch3Grid* Grid;

	/** Location on the grid as a 1D key/value. To find the neighbors of this tile, ask the grid. */
	UPROPERTY(BlueprintReadOnly, Category = "Match 3 Tile")
	int32 GridAddress;

	/** Location on the grid where the tile is falling towards. */
	UPROPERTY(BlueprintReadOnly, Category = "Match 3 Tile")
	int32 LandingGridAddress;
};

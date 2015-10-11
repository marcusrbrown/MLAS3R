// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Match3GridTile.h"
#include "Match3Grid.generated.h"

UCLASS()
class MLAS3R_API AMatch3Grid : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMatch3Grid();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	AMatch3GridTile* GetTileFromGridAddress(int32 GridAddress) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (ExpandEnumAsExecs = "MoveType"), Category = "Match 3 Tile")
	void OnMoveMade(EMatch3MoveType MoveType);

	UPROPERTY(VisibleAnywhere)
	TArray<AMatch3GridTile*> Tiles;

	/** The width of the grid. Used to calculate tile positions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Tile")
	int32 GridWidth;

	/** The height of the grid. Used to calculate tile positions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Tile")
	int32 GridHeight;

	/** The minimum number of tiles required to make a match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Tile")
	int32 MinimumMatchLength;

	/** The size of a space on the grid. The size does not include borders or the spacing between tiles. */
	UPROPERTY(EditAnywhere, Category = "Match 3 Tile")
	FVector2D TileSize;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Match3GridTile.h"
#include "Match3Grid.generated.h"

/** TODO: This guy will not survive. He's a load-bearing struct from the Match 3 training series. */
USTRUCT()
struct FTileType
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Probability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* TileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileAbilities Abilities;

	FTileType()
	: Probability(1.0f)
	{
	}
};

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

	AMatch3GridTile* CreateTile(UStaticMesh* StaticMesh, FVector SpawnLocation, int32 SpawnGridAddress, int TileTypeID);

	int32 SelectTileFromLibrary() const;

	AMatch3GridTile* GetTileFromGridAddress(int32 GridAddress) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (ExpandEnumAsExecs = "MoveType"), Category = "Match 3 Tile")
	void OnMoveMade(EMatch3MoveType MoveType);

	/** Get the world location from any grid address. */
	UFUNCTION(BlueprintCallable, Category = "Match 3 Tile")
	FVector GetLocationFromGridAddress(int32 GridAddress);

	FVector GetLocationFromGridAddress(int32 GridAddress, int32 XOffsetInTiles, int32 YOffsetInTiles);

	/** Get a grid address relative to another grid address. */
	UFUNCTION(BlueprintCallable, Category = "Match 3 Tile")
	bool GetGridAddressWithOffset(int32 GridAddress, int32 XOffset, int32 YOffset, int32& RelativeGridAddress) const;

	bool AreAddressesNeighbors(int32 GridAddressA, int32 GridAddressB) const;

	void OnTileFallingFinished(AMatch3GridTile* Tile, int32 LandingGridAddress);
	void OnTileMatchingFinished(AMatch3GridTile* Tile);
	void OnSwapDisplayFinished(AMatch3GridTile* Tile);

	void RespawnTiles();
	void SwapTiles(AMatch3GridTile* TileA, AMatch3GridTile* TileB, bool bRepositionTileActors = false);
	bool IsMoveLegal(AMatch3GridTile* TileA, AMatch3GridTile* TileB);

	/** Check for a successful sequence. */
	TArray<AMatch3GridTile*> FindNeighbors(AMatch3GridTile* StartingTile, bool bMustMatchID = true, int32 MatchLength = -1) const;

	/** Find all tiles of a given type. */
	TArray<AMatch3GridTile*> FindTilesOfType(int32 TileTypeID) const;

	/** Executes the result of one or more matches. */
	void ExecuteMatch(TArray<AMatch3GridTile*> MatchingTiles);

	/** Reacts to a tile being clicked. */
	void OnTileWasSelected(AMatch3GridTile* NewSelectedTile);

	/** Detects unwinnable states. */
	bool IsUnwinnable() const;

	/** Gets the last move made. */
	EMatch3MoveType GetLastMove() const;

	/** Establishes the most recent move type. */
	void SetLastMove(EMatch3MoveType MoveType);

	UFUNCTION(BlueprintNativeEvent, Category = Game)
	int32 GetScoreMultiplier(EMatch3MoveType MoveType);
	virtual int32 GetScoreMultiplier_Implementation(EMatch3MoveType MoveType);

	UPROPERTY(VisibleAnywhere)
	TArray<AMatch3GridTile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTileType> TileLibrary;

	UPROPERTY(EditAnywhere, Category = "Match 3 Tile")
	TSubclassOf<class AMatch3GridTile> TileToSpawn;

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

	AMatch3GridTile* SelectedTile;

private:
	/** Array of tiles found in the most recent call to IsMoveLegal(). */
	TArray<AMatch3GridTile*> LastLegalMatch;

	/** Tiles that are currently falling. */
	TArray<AMatch3GridTile*> FallingTiles;

	/** Tiles that are currently being swapped. */
	TArray<AMatch3GridTile*> SwappingTiles;

	/** Tiles that should be checked for automatic matches. */
	TArray<AMatch3GridTile*> TilesToCheck;

	/** Tiles that are currently being destroyed (because matches). */
	TArray<AMatch3GridTile*> TilesBeingDestroyed;

	/** The last move made by a player. */
	TMap<APlayerController*, EMatch3MoveType> LastMoves;

	/** Indicates that we are waiting to complete a swap move. */
	uint32 bPendingSwapMove : 1;

	/** Indicates that the pending swap move will succeed. */
	uint32 bPendingSwapMoveSuccess : 1;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Match3/Match3GridTile.h"
#include "Match3Grid.generated.h"

/** Describes tiles stored within the tile library. */
USTRUCT()
struct FTileLibraryDescriptor
{
    GENERATED_BODY()

    /** The probability that the tile will spawn. This value is normalized across all tile descriptors in a tile library. */
	UPROPERTY(EditInstanceOnly)
	float Probability;

    /** The tile class used to spawn instances of this tile on the grid. */
    UPROPERTY(EditInstanceOnly)
    TSubclassOf<class AMatch3BasicTile> TileClass;

    FTileLibraryDescriptor()
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

	AMatch3GridTile* CreateTile(UClass* TileClass, FVector SpawnLocation, int32 SpawnGridAddress, int TileTypeID);
    class AMatch3EnemyTile* CreateEnemyTile(UClass* TileClass, FVector SpawnLocation, int32 SpawnGridAddress, class AEnemy* Enemy, FName MatchId);

    void FillTilesFromCapturedActors();
    void FillTilesFromLibrary();
    int32 SelectTileFromLibrary() const;

    /** Capture the actors that will be used to fill in the Match 3 grid. */
    UFUNCTION(BlueprintCallable, Category = "Match 3 Grid")
    void CaptureActors(TArray<AActor*> Actors);

    UFUNCTION(BlueprintNativeEvent, Category = "Match 3 Grid")
    void OnEnemiesCaptured(TArray<class AMatch3EnemyTile*> const& EnemyTiles, TArray<class AEnemy*> const& CapturedEnemies);
    virtual void OnEnemiesCaptured_Implementation(TArray<class AMatch3EnemyTile*> const& EnemyTiles, TArray<class AEnemy*> const& CapturedEnemies);

    UFUNCTION(BlueprintNativeEvent, Category = "Match 3 Grid")
    void OnEnemiesReleased(TArray<class AMatch3EnemyTile*> const& EnemyTiles, TArray<class AEnemy*> const& CapturedEnemies);
    virtual void OnEnemiesReleased_Implementation(TArray<class AMatch3EnemyTile*> const& EnemyTiles, TArray<class AEnemy*> const& CapturedEnemies);

    /** Toggle the grid. When enabled, all contained tiles are visible and ticking. */
    UFUNCTION(BlueprintCallable, Category = "Match 3 Grid")
    void ToggleGrid(bool bEnabled);

    UFUNCTION(BlueprintNativeEvent, Category = "Match 3 Grid")
    void OnGridActivated();
    virtual void OnGridActivated_Implementation();

    UFUNCTION(BlueprintNativeEvent, Category = "Match 3 Grid")
    void OnGridDeactivated();
    virtual void OnGridDeactivated_Implementation();

	AMatch3GridTile* GetTileFromGridAddress(int32 GridAddress) const;

	UFUNCTION(BlueprintImplementableEvent, meta = (ExpandEnumAsExecs = "MoveType"), Category = "Match 3 Grid")
	void OnMoveMade(EMatch3MoveType MoveType);

	/** Get the world location from any grid address. */
	UFUNCTION(BlueprintPure, Category = "Match 3 Grid")
	FVector GetLocationFromGridAddress(int32 GridAddress);

	FVector GetLocationFromGridAddress(int32 GridAddress, int32 XOffsetInTiles, int32 YOffsetInTiles);

	/** Get a grid address relative to another grid address. */
	UFUNCTION(BlueprintCallable, Category = "Match 3 Grid")
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
	EMatch3MoveType GetLastMove();

	/** Establishes the most recent move type. */
	void SetLastMove(EMatch3MoveType MoveType);

	UFUNCTION(BlueprintNativeEvent, Category = Game)
	int32 GetScoreMultiplier(EMatch3MoveType MoveType);
	virtual int32 GetScoreMultiplier_Implementation(EMatch3MoveType MoveType);

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match 3 Grid")
	TArray<AMatch3GridTile*> Tiles;

	UPROPERTY(EditInstanceOnly, Category = "Match 3 Grid")
	TArray<FTileLibraryDescriptor> TileLibrary;

	UPROPERTY(EditAnywhere, Category = "Match 3 Grid")
	TSubclassOf<class AMatch3EnemyTile> EnemyTileClass;

	/** The width of the grid. Used to calculate tile positions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Grid")
	int32 GridWidth;

	/** The height of the grid. Used to calculate tile positions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Grid")
	int32 GridHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Grid")
	FVector GridOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Grid")
	FVector GridPadding;

	/** The minimum number of tiles required to make a match. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Tile")
	int32 MinimumMatchLength;

	/** The size of a space on the grid. The size does not include borders or the spacing between tiles. */
	UPROPERTY(EditAnywhere, Category = "Match 3 Tile")
	FVector2D TileSize;

    UPROPERTY()
	AMatch3GridTile* SelectedTile;

private:
    void ToggleEnemyTileSync(bool bEnabled);

	UPROPERTY(Category = "Bounds", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Bounds;

    UPROPERTY()
	/** Array of tiles found in the most recent call to IsMoveLegal(). */
	TArray<AMatch3GridTile*> LastLegalMatch;

    UPROPERTY()
	/** Tiles that are currently falling. */
	TArray<AMatch3GridTile*> FallingTiles;

    UPROPERTY()
	/** Tiles that are currently being swapped. */
	TArray<AMatch3GridTile*> SwappingTiles;

    UPROPERTY()
	/** Tiles that should be checked for automatic matches. */
	TArray<AMatch3GridTile*> TilesToCheck;

    UPROPERTY()
	/** Tiles that are currently being destroyed (because matches). */
	TArray<AMatch3GridTile*> TilesBeingDestroyed;

    UPROPERTY()
    TArray<AActor*> CapturedActors;

    UPROPERTY()
    TArray<class AMatch3EnemyTile*> EnemyTiles;

    UPROPERTY()
    TArray<class AEnemy*> CapturedEnemies;

	/** The last move made by a player. */
	TMap<APlayerController*, EMatch3MoveType> LastMoves;

	/** Indicates that we are waiting to complete a swap move. */
	uint32 bPendingSwapMove : 1;

	/** Indicates that the pending swap move will succeed. */
	uint32 bPendingSwapMoveSuccess : 1;
};

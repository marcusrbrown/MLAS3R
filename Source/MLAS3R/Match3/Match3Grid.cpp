// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Match3Grid.h"
#include "Match3/Match3BasicTile.h"
#include "Match3/Match3EnemyTile.h"
#include "MLAS3RPlayerController.h"
#include "Enemy.h"

namespace
{
	static const FColor DefaultBoundsColor(100, 255, 100, 255);
	static const FRotator DefaultSpawnRotation(0.0f, 90.0f, 0.0f);
}

// Sets default values
AMatch3Grid::AMatch3Grid()
: GridWidth(6)
, GridHeight(6)
, MinimumMatchLength(3)
, TileSize(25.0f, 25.0f)
, SelectedTile(nullptr)
, bPendingSwapMove(false)
, bPendingSwapMoveSuccess(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup our RootComponent
	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Bounds->ShapeColor = DefaultBoundsColor;
	Bounds->InitBoxExtent(FVector(40.0f, 40.0f, 40.0f));
	RootComponent = Bounds;
}

// Called when the game starts or when spawned
void AMatch3Grid::BeginPlay()
{
	Super::BeginPlay();

	// Position the grid.
	GridOffset += GetActorLocation() - Bounds->GetScaledBoxExtent();
	float gridExtentX = TileSize.X * GridWidth + (GridPadding.X * GridWidth);
	GridOffset.X += ((Bounds->GetScaledBoxExtent().X * 2.0f) - gridExtentX) * 0.5f;

	Tiles.Empty(GridWidth * GridHeight);
	Tiles.AddDefaulted(Tiles.Max());
}

// Called every frame
void AMatch3Grid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

AMatch3GridTile* AMatch3Grid::CreateTile(UClass* TileClass, FVector SpawnLocation, int32 SpawnGridAddress, int TileTypeID)
{
	checkSlow(TileLibrary.IsValidIndex(TileTypeID));
	checkSlow(TileLibrary[TileTypeID] != nullptr);

	auto* const world = GetWorld();
	if (world)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		spawnParams.Instigator = Instigator;
        spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		auto* const newTile = world->SpawnActor<AMatch3GridTile>(TileClass, SpawnLocation, DefaultSpawnRotation, spawnParams);
		newTile->TileTypeID = TileTypeID;
		newTile->SetGridAddress(SpawnGridAddress);
        newTile->SetActorHiddenInGame(true);
		Tiles[SpawnGridAddress] = newTile;
		return newTile;
	}

	return nullptr;
}

AMatch3EnemyTile* AMatch3Grid::CreateEnemyTile(UClass* TileClass, FVector SpawnLocation, int32 SpawnGridAddress, AEnemy* Enemy, FName MatchId)
{
    auto* const world = GetWorld();
    if (world)
    {
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = this;
        spawnParams.Instigator = Instigator;
        spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        auto* const newTile = world->SpawnActor<AMatch3EnemyTile>(TileClass, SpawnLocation, DefaultSpawnRotation, spawnParams);
        newTile->SetEnemy(Enemy);
        newTile->MatchId = MatchId;
        newTile->SetGridAddress(SpawnGridAddress);
        Tiles[SpawnGridAddress] = newTile;
        EnemyTiles.Add(newTile);
        return newTile;
    }

    return nullptr;
}

void AMatch3Grid::FillTilesFromCapturedActors()
{
    if (CapturedActors.Num() == 0)
    {
        return;
    }

    for (auto actor : CapturedActors)
    {
        auto enemy = Cast<AEnemy>(actor);

        if (enemy == nullptr)
        {
            continue;
        }

        auto color = enemy->Color;
        FString colorName = GetEnemyColorAsString(color);
        auto matchId = FName(*FString::Printf(TEXT("Match %s"), *colorName));
        AMatch3EnemyTile* enemyTile = nullptr;

        for (auto tile : Tiles)
        {
            if (!tile->IsA(AMatch3EnemyTile::StaticClass()) && (tile->MatchId == matchId))
            {
                // Replace the non-enemy tile with a new enemy tile.
                auto spawnGridAddress = tile->GetGridAddress();
                auto spawnLocation = GetLocationFromGridAddress(spawnGridAddress);

                enemyTile = CreateEnemyTile(EnemyTileClass, spawnLocation, spawnGridAddress, enemy, matchId);
                GetWorld()->DestroyActor(tile);
                break;
            }
        }

        if (enemyTile == nullptr)
        {
            CapturedEnemies.Add(enemy);
        }
    }
}

void AMatch3Grid::FillTilesFromLibrary()
{
    for (int32 column = 0; column < GridWidth; ++column)
    {
        for (int32 row = 0; row < GridHeight; ++row)
        {
            int32 gridAddress;
            GetGridAddressWithOffset(0, column, row, gridAddress);

            if (GetTileFromGridAddress(gridAddress) != nullptr)
            {
                // There's already a tile assigned to this location.
                continue;
            }

            UClass* tileClass = nullptr;
            int32 tileID;
            for (;;)
            {
                tileID = SelectTileFromLibrary();
                tileClass = TileLibrary[tileID].TileClass;

                if (tileClass == nullptr)
                {
                    break;
                }

                auto matchId = tileClass->GetDefaultObject<AMatch3GridTile>()->MatchId;

                // TODO: marcus@HV: Reflow this logic (originally from the Match 3 training video.

                if ((column >= MinimumMatchLength - 1) || (row >= MinimumMatchLength - 1))
                {
                    int32 testAddress = 0;
                    int32 tileOffset = 0;

                    for (int32 horizontal = 0; horizontal < 2; ++horizontal)
                    {
                        for (tileOffset = 1; tileOffset < MinimumMatchLength; ++tileOffset)
                        {
                            // TODO: marcus@HV: Epic had this in their training code, I don't think it's valid to call this before the GetGridAddressWithOffset() call below.
                            //checkSlow(GetTileFromGridAddress(testAddress));

                            if (!GetGridAddressWithOffset(0, column - (horizontal ? tileOffset : 0), row - (horizontal ? 0 : tileOffset), testAddress)
                                || (GetTileFromGridAddress(testAddress)->MatchId != matchId))
                            {
                                // Not in a matching run, or off the edge of a map, so stop checking this axis.
                                break;
                            }
                        }

                        if (tileOffset == MinimumMatchLength)
                        {
                            // We made it through the whole "check for matching run" loop. This tile completes a scoring run. Pick a new tile type and test again.
                            break;
                        }
                    }

                    if (tileOffset < MinimumMatchLength)
                    {
                        // We didn't find a matching run in either direction, so we can place a tile at this location.
                        break;
                    }
                }
                else
                {
                    // This tile is too close to the edge to be worth checking.
                    break;
                }
            }

            if (tileClass != nullptr)
            {
                FVector spawnLocation = GetLocationFromGridAddress(gridAddress);

                CreateTile(tileClass, spawnLocation, gridAddress, tileID);
            }
        }
    }
}

int32 AMatch3Grid::SelectTileFromLibrary() const
{
	float normalizingFactor = 0.0f;
	for (auto& tileBase : TileLibrary)
	{
		normalizingFactor += tileBase.Probability;
	}

	float testNumber = FMath::RandRange(0.0f, normalizingFactor);
	float compareTo = 0.0f;
	for (int32 arrayChecked = 0; arrayChecked != TileLibrary.Num(); ++arrayChecked)
	{
		compareTo += TileLibrary[arrayChecked].Probability;

		if (testNumber <= compareTo)
		{
			return arrayChecked;
		}
	}

	return 0;
}

void AMatch3Grid::CaptureActors(TArray<AActor*> Actors)
{
    CapturedActors = Actors;
}

void AMatch3Grid::OnEnemiesCaptured_Implementation(TArray<class AMatch3EnemyTile*> const& EnemyTiles, TArray<class AEnemy*> const& CapturedEnemies)
{
    ToggleEnemyTileSync(true);

    OnGridActivated();
}

void AMatch3Grid::OnEnemiesReleased_Implementation(TArray<class AMatch3EnemyTile*> const & EnemyTiles, TArray<class AEnemy*> const & CapturedEnemies)
{
    OnGridDeactivated();
}

void AMatch3Grid::ToggleEnemyTileSync(bool bEnabled)
{
    for (auto tile : Tiles)
    {
        auto enemyTile = Cast<AMatch3EnemyTile>(tile);

        if (enemyTile != nullptr)
        {
            enemyTile->bSyncEnemyLocation = bEnabled;
            //enemyTile->GetEnemy()->SetActorHiddenInGame(!bEnabled);
        }
    }
}

void AMatch3Grid::ToggleGrid(bool bEnabled)
{
    if (bEnabled)
    {
        Tiles.Empty(GridWidth * GridHeight);
        Tiles.AddDefaulted(Tiles.Max());

        FillTilesFromLibrary();
        FillTilesFromCapturedActors();

        OnEnemiesCaptured(EnemyTiles, CapturedEnemies);
    }
    else
    {
        LastLegalMatch.Empty();
        FallingTiles.Empty();
        SwappingTiles.Empty();
        TilesToCheck.Empty();
        TilesBeingDestroyed.Empty();

        CapturedActors.Empty();

        bPendingSwapMove = bPendingSwapMoveSuccess = false;

        ToggleEnemyTileSync(false);

        OnEnemiesReleased(EnemyTiles, CapturedEnemies);
    }
}

void AMatch3Grid::OnGridActivated_Implementation()
{
    for (auto tile : Tiles)
    {
        if (IsValid(tile))
        {
            tile->SetActorHiddenInGame(false);
        }
    }
}

void AMatch3Grid::OnGridDeactivated_Implementation()
{
    for (auto tile : Tiles)
    {
        if (IsValid(tile))
        {
            GetWorld()->DestroyActor(tile);
        }
    }

    EnemyTiles.Empty();
    CapturedEnemies.Empty();
    SelectedTile = nullptr;

    Tiles.Empty(GridWidth * GridHeight);
    Tiles.AddDefaulted(Tiles.Max());
}


AMatch3GridTile* AMatch3Grid::GetTileFromGridAddress(int32 GridAddress) const
{
	checkSlow(GridWidth > 0);
	checkSlow(GridHeight > 0);

	if ((GridAddress >= 0) && (GridAddress < (GridWidth * GridHeight)))
	{
		return Tiles[GridAddress];
	}

	return nullptr;
}

FVector AMatch3Grid::GetLocationFromGridAddress(int32 GridAddress)
{
	return GetLocationFromGridAddress(GridAddress, 0, 0);
}

FVector AMatch3Grid::GetLocationFromGridAddress(int32 GridAddress, int32 XOffsetInTiles, int32 YOffsetInTiles)
{
	checkSlow(TileSize.X > 0.0f);
	checkSlow(TileSize.Y > 0.0f);
	checkSlow(GridWidth > 0);
	checkSlow(GridHeight > 0);

	int32 relativeGridAddress;
	if (!GetGridAddressWithOffset(GridAddress, XOffsetInTiles, YOffsetInTiles, relativeGridAddress))
	{
		// TODO: marcus@HV: Rework this logic.
	}

	int32 column = GridAddress % GridWidth;
	int32 row = GridAddress / GridWidth;
	float x = GridOffset.X + (column * TileSize.X + (GridPadding.X * column));
	float y = GridOffset.Y + (row * TileSize.Y + (GridPadding.Y * row));

	x += TileSize.X * 0.5f;
	y += TileSize.Y * 0.5f;

	return FVector(x, y, 0.0f);
}

bool AMatch3Grid::GetGridAddressWithOffset(int32 GridAddress, int32 XOffset, int32 YOffset, int32& RelativeGridAddress) const
{
	checkSlow(GridWidth > 0);
	checkSlow(GridHeight > 0);
	checkSlow((GridAddress >= 0) && (GridAddress < (GridWidth * GridHeight)));

	// Convert the grid address back to 2D coordinates before applying the offsets.
	int32 column = GridAddress % GridWidth;
	int32 row = GridAddress / GridWidth;
	column += XOffset;
	row += YOffset;

	if ((column >= 0) && (column < GridWidth) && (row >= 0) && (row < GridHeight))
	{
		RelativeGridAddress = row * GridWidth + column;
		return true;
	}

	return false;
}

bool AMatch3Grid::AreAddressesNeighbors(int32 GridAddressA, int32 GridAddressB) const
{
	return false;
}

void AMatch3Grid::OnTileFallingFinished(AMatch3GridTile* Tile, int32 LandingGridAddress)
{
	int32 returnGridAddress;

	if (GetGridAddressWithOffset(Tile->GetGridAddress(), 0, 0, returnGridAddress))
	{
		// Remove the tile from it's original position.
		if (Tiles[returnGridAddress] == Tile)
		{
			Tiles[returnGridAddress] = nullptr;
		}
	}

	if (GetGridAddressWithOffset(LandingGridAddress, 0, 0, returnGridAddress))
	{
		Tiles[returnGridAddress] = Tile;
		// TODO: marcus@HV: Verify.
		Tile->SetGridAddress(returnGridAddress);
	}

    FallingTiles.Remove(Tile);
    TilesToCheck.Add(Tile);

    if (FallingTiles.Num() == 0)
    {
        // Now that all tiles have finished falling, check for cascading matches.
        for (auto checkTile : TilesToCheck)
        {
            // TODO: TJW - Investigate Crash
            if (!IsValid(checkTile)) continue;

            LastLegalMatch = FindNeighbors(checkTile);

            if (LastLegalMatch.Num() > 0)
            {
                ExecuteMatch(LastLegalMatch);
                return;
            }
        }

        TilesToCheck.Empty();
    }
}

void AMatch3Grid::OnTileMatchingFinished(AMatch3GridTile* Tile)
{
    if (IsValid(Tile))
    {
        auto enemyTile = Cast<AMatch3EnemyTile>(Tile);

        if (enemyTile != nullptr)
        {
            EnemyTiles.Remove(enemyTile);
        }

        // Remove the tile.
        GetWorld()->DestroyActor(Tile);
    }
}

void AMatch3Grid::OnSwapDisplayFinished(AMatch3GridTile* Tile)
{
	SwappingTiles.Add(Tile);

	if (SwappingTiles.Num() == 2)
	{
		checkSlow(SwappingTiles[0] && SwappingTiles[1]);

		bPendingSwapMove = false;

		if (bPendingSwapMoveSuccess)
		{
			SwapTiles(SwappingTiles[0], SwappingTiles[1], true);
			SetLastMove(EMatch3MoveType::Swap);
			ExecuteMatch(LastLegalMatch);
		}
		else
		{
			OnMoveMade(EMatch3MoveType::Invalid);
		}

		SwappingTiles.Empty();
	}
}

void AMatch3Grid::RespawnTiles()
{

}

void AMatch3Grid::SwapTiles(AMatch3GridTile* TileA, AMatch3GridTile* TileB, bool bRepositionTileActors)
{
	int32 gridAddress = TileA->GetGridAddress();

	TileA->SetGridAddress(TileB->GetGridAddress());
	TileB->SetGridAddress(gridAddress);

	Tiles[TileA->GetGridAddress()] = TileA;
	Tiles[TileB->GetGridAddress()] = TileB;

	if (bRepositionTileActors)
	{
		TileA->SetActorLocation(GetLocationFromGridAddress(TileA->GetGridAddress()));
		TileB->SetActorLocation(GetLocationFromGridAddress(TileB->GetGridAddress()));
	}
}

bool AMatch3Grid::IsMoveLegal(AMatch3GridTile* TileA, AMatch3GridTile* TileB)
{
	if (TileA && TileB && (TileA != TileB) && TileA->Abilities.CanSwap() && TileB->Abilities.CanSwap())
	{
		if (TileA->MatchId != TileB->MatchId)
		{
			// Swap the tiles, check for matches, then swap them back to determine whether the move is legal.
			SwapTiles(TileA, TileB);

			LastLegalMatch = FindNeighbors(TileA);
			LastLegalMatch.Append(FindNeighbors(TileB));

			SwapTiles(TileA, TileB);

			return LastLegalMatch.Num() > 0;
		}
	}

	return false;
}

TArray<AMatch3GridTile*> AMatch3Grid::FindNeighbors(AMatch3GridTile* StartingTile, bool bMustMatchID, int32 MatchLength) const
{
	if (MatchLength < 0)
	{
		MatchLength = MinimumMatchLength;
	}

	TArray<AMatch3GridTile*> allMatchingTiles;

	for (int32 horizontal = 0; horizontal < 2; ++horizontal)
	{
		TArray<AMatch3GridTile*> matchInProgress;

		for (int32 direction = -1; direction <= 1; direction += 2)
		{
			int32 maxGridOffset = !bMustMatchID ? MatchLength : (horizontal ? GridWidth : GridHeight);

			for (int32 gridOffset = 1; gridOffset < maxGridOffset; ++gridOffset)
			{
				int32 neighborGridAddress;
				AMatch3GridTile* neighborTile;

				if (GetGridAddressWithOffset(StartingTile->GetGridAddress(), direction * (horizontal ? gridOffset : 0), direction * (horizontal ? 0 : gridOffset), neighborGridAddress))
				{
					neighborTile = GetTileFromGridAddress(neighborGridAddress);

					if (neighborTile && (!bMustMatchID || (neighborTile->MatchId == StartingTile->MatchId)))
					{
						matchInProgress.Add(neighborTile);
						continue;
					}

					break;
				}
			}
		}

		if (!bMustMatchID || ((matchInProgress.Num() + 1) >= FMath::Min(MatchLength, horizontal ? GridWidth : GridHeight)))
		{
			allMatchingTiles.Append(matchInProgress);
		}
	}

	if (!bMustMatchID || (allMatchingTiles.Num() > 0))
	{
		allMatchingTiles.Add(StartingTile);
	}

	return allMatchingTiles;
}

TArray<AMatch3GridTile*> AMatch3Grid::FindTilesOfType(int32 TileTypeID) const
{
	return TArray<AMatch3GridTile*>();
}

void AMatch3Grid::ExecuteMatch(TArray<AMatch3GridTile*> MatchingTiles)
{
    for (auto tile : MatchingTiles)
    {
        // TODO: marcus@HV: Handle scoring here.

        tile->TileState = EMatch3TileState::Removed;
    }

    for (auto tile : MatchingTiles)
    {
        // Set all tiles above this tile to fall, unless they were removed or already falling.
        for (int32 yOffset = 1; yOffset < GridHeight; ++yOffset)
        {
            int32 aboveGridAddress;
            if (GetGridAddressWithOffset(tile->GetGridAddress(), 0, -yOffset, aboveGridAddress))
            {
                if (auto tileAbove = GetTileFromGridAddress(aboveGridAddress))
                {
                    if ((tileAbove->TileState != EMatch3TileState::Removed)
                        && (tileAbove->TileState != EMatch3TileState::Falling))
                    {
                        FallingTiles.Add(tileAbove);
                    }
                }
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("This one"));
            }
        }

        tile->OnMatched(GetLastMove());
    }

    for (auto tile : FallingTiles)
    {
        tile->StartFalling();
    }
}

void AMatch3Grid::OnTileWasSelected(AMatch3GridTile* NewSelectedTile)
{
	if (FallingTiles.Num() || TilesBeingDestroyed.Num() || bPendingSwapMove)
	{
		return;
	}

	checkSlow(NewSelectedTile);
#if 0 // TODO: marcus@HV: EMJ last minute hack.
	checkSlow(TileLibrary.IsValidIndex(NewSelectedTile->TileTypeID));
	checkSlow(TileLibrary[NewSelectedTile->TileTypeID] != nullptr);
#endif

	if (SelectedTile)
	{
		if (SelectedTile == NewSelectedTile)
		{
			SelectedTile->PlaySelectionEffect(false);
			SelectedTile = nullptr;
			return;
		}

		// TODO: marcus@HV: We could check for valid neighbors here, this is how Bejeweled Match 3 games work. You skip that sort of check when you can move freely across the grid.
		if (NewSelectedTile->Abilities.CanSwap())
		{
			bPendingSwapMove = true;
			bPendingSwapMoveSuccess = IsMoveLegal(SelectedTile, NewSelectedTile);
			SelectedTile->OnSwapMove(NewSelectedTile, bPendingSwapMoveSuccess);
			NewSelectedTile->OnSwapMove(SelectedTile, bPendingSwapMoveSuccess);
		}
		else
		{
			OnMoveMade(EMatch3MoveType::Invalid);
		}

		SelectedTile->PlaySelectionEffect(false);
		SelectedTile = nullptr;
	}
	else
	{
		// TODO: marcus@HV: Here Bejeweled Match 3 games could check for things like bombs, etc.

		if (NewSelectedTile->Abilities.CanSwap())
		{
			// This is the first tile in the sequence, so remember it for later.
			SelectedTile = NewSelectedTile;
			SelectedTile->PlaySelectionEffect(true);
		}
		else
		{
			// TODO: marcus@HV: We could farm out different abilities to blueprints.
			// Invalid move because the selected tile type doesn't have any abilities.
			OnMoveMade(EMatch3MoveType::Invalid);
		}
	}
}

bool AMatch3Grid::IsUnwinnable() const
{
	return false;
}

EMatch3MoveType AMatch3Grid::GetLastMove()
{
    // TODO: marcus@HV: Move the logic to get the custom player controller into a shared location.
    auto pc = UGameplayStatics::GetPlayerController(this, 0);
    auto ourPc = Cast<AMLAS3RPlayerController>(pc);

    if (ourPc != nullptr)
    {
        return ourPc->LastMove;
    }

    return EMatch3MoveType::None;
}

void AMatch3Grid::SetLastMove(EMatch3MoveType MoveType)
{
    // TODO: marcus@HV: Move the logic to get the custom player controller into a shared location.
    auto pc = UGameplayStatics::GetPlayerController(this, 0);
    auto ourPc = Cast<AMLAS3RPlayerController>(pc);

    if (ourPc != nullptr)
    {
        ourPc->LastMove = MoveType;
    }
}

int32 AMatch3Grid::GetScoreMultiplier_Implementation(EMatch3MoveType MoveType)
{
	return 0;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Match3Grid.h"


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
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMatch3Grid::BeginPlay()
{
	Super::BeginPlay();

	Tiles.Empty(GridWidth * GridHeight);
	Tiles.AddUninitialized(Tiles.Max());

	for (int32 column = 0; column < GridWidth; ++column)
	{
		for (int32 row = 0; row < GridHeight; ++row)
		{
			int32 gridAddress;
			GetGridAddressWithOffset(0, column, row, gridAddress);
			FVector spawnLocation = GetLocationFromGridAddress(gridAddress);

			int32 tileID;
			for (;;)
			{
				tileID = SelectTileFromLibrary();

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

							if (!GetGridAddressWithOffset(0, column - (horizontal ? tileOffset : 0), row - (horizontal ? 0 : tileOffset), testAddress) || (GetTileFromGridAddress(testAddress)->TileTypeID != tileID))
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

			CreateTile(nullptr, spawnLocation, gridAddress, tileID);
		}
	}
}

// Called every frame
void AMatch3Grid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

AMatch3GridTile* AMatch3Grid::CreateTile(AActor* TileActor, FVector SpawnLocation, int32 SpawnGridAddress, int TileTypeID)
{
	if (TileToSpawn)
	{
		checkSlow(TileLibrary.IsValidIndex(TileTypeID));
		checkSlow(TileLibrary[TileTypeID] != nullptr);

		auto* const world = GetWorld();
		if (world)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;
			spawnParams.Instigator = Instigator;

			// TODO: marcus@HV: Tiles currently don't rotate.
			FRotator spawnRotation = FRotator(0.0f, 0.0f, 0.0f);
			auto* const newTile = world->SpawnActor<AMatch3GridTile>(TileToSpawn, SpawnLocation, spawnRotation, spawnParams);
			// TODO: marcus@HV: Set up rendering.
			newTile->TileTypeID = TileTypeID;
			newTile->Abilities = TileLibrary[TileTypeID].Abilities;
			newTile->SetGridAddress(SpawnGridAddress);
			Tiles[SpawnGridAddress] = newTile;
			return newTile;
		}
	}

	return nullptr;
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
	float x = GetActorLocation().X + (column * TileSize.X);
	float y = GetActorLocation().Y + (row * TileSize.Y);

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
}

void AMatch3Grid::OnTileMatchingFinished(AMatch3GridTile* Tile)
{

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
		if (TileA->TileTypeID != TileB->TileTypeID)
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

					if (neighborTile && (!bMustMatchID || (neighborTile->TileTypeID == StartingTile->TileTypeID)))
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

}

void AMatch3Grid::OnTileWasSelected(AMatch3GridTile* NewSelectedTile)
{
	if (FallingTiles.Num() || TilesBeingDestroyed.Num() || bPendingSwapMove)
	{
		return;
	}

	checkSlow(NewSelectedTile);
	checkSlow(TileLibrary.IsValidIndex(NewSelectedTile->TileTypeID));
	checkSlow(TileLibrary[NewSelectedTile->TileTypeID] != nullptr);
	FTileType& NewSelectedTileType = TileLibrary[NewSelectedTile->TileTypeID];

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

		if (NewSelectedTileType.Abilities.CanSwap())
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

EMatch3MoveType AMatch3Grid::GetLastMove() const
{
	return EMatch3MoveType::None;
}

void AMatch3Grid::SetLastMove(EMatch3MoveType MoveType)
{

}

int32 AMatch3Grid::GetScoreMultiplier_Implementation(EMatch3MoveType MoveType)
{
	return 0;
}

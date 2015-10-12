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
}

// Called every frame
void AMatch3Grid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

AMatch3GridTile* AMatch3Grid::GetTileFromGridAddress(int32 GridAddress) const
{
	return nullptr;
}

FVector AMatch3Grid::GetLocationFromGridAddress(int32 GridAddress)
{
	return FVector();
}

FVector AMatch3Grid::GetLocationFromGridAddress(int32 GridAddress, int32 XOffsetInTiles, int32 YOffsetInTiles)
{
	return FVector();
}

bool AMatch3Grid::GetGridAddressWithOffset(int32 GridAddress, int32 XOffset, int32 YOffset, int32& RelativeGridAddress) const
{
	return false;
}

bool AMatch3Grid::AreAddressesNeighbors(int32 GridAddressA, int32 GridAddressB) const
{
	return false;
}

void AMatch3Grid::OnTileFallingFinished(AMatch3GridTile* Tile, int32 LandingGridAddress)
{

}

void AMatch3Grid::OnTileMatchingFinished(AMatch3GridTile* Tile)
{

}

void AMatch3Grid::OnTileSwappingFinished(AMatch3GridTile* Tile)
{

}

void AMatch3Grid::RespawnTiles()
{

}

void AMatch3Grid::SwapTiles(AMatch3GridTile* TileA, AMatch3GridTile* TileB, bool bRepositionTileActors)
{

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

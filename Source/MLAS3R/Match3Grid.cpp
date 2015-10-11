// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Match3Grid.h"


// Sets default values
AMatch3Grid::AMatch3Grid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMatch3Grid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMatch3Grid::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FVector AMatch3Grid::GetLocationFromGridAddress(int32 GridAddress)
{
	return FVector();
}

FVector AMatch3Grid::GetLocationFromGridAddress(int32 GridAddress, int32 XOffsetInTiles, int32 YOffsetInTiles)
{
	return FVector();
}

bool AMatch3Grid::GetGridAddressWithOffset(int32 GridAddress, int32 XOffset, int32 YOffset, int32& RelativeGridAddress)
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

bool AMatch3Grid::IsMoveLegal(AMatch3GridTile* TileA, AMatch3GridTile* TileB) const
{
	return false;
}

TArray<AMatch3GridTile*> AMatch3Grid::FindNeighbors(AMatch3GridTile* StartingTile, bool bMustMatchID, int32 MatchLength) const
{
	return TArray<AMatch3GridTile*>();
}

TArray<AMatch3GridTile*> AMatch3Grid::FindTilesOfType(int32 TileTypeID) const
{
	return TArray<AMatch3GridTile*>();
}

void AMatch3Grid::ExecuteMatch(TArray<AMatch3GridTile*> MatchingTiles)
{

}

void AMatch3Grid::OnTileWasSelected(AMatch3GridTile* Tile)
{

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

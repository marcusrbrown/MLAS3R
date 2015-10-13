// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "MLAS3RGameMode.h"
#include "Match3Grid.h"
#include "Match3GridTile.h"


// Sets default values
AMatch3GridTile::AMatch3GridTile()
: FallingStartTime(0.0f)
, TotalFallingTime(0.0f)
, FallingStartLocation(0.0f)
, FallingEndLocation(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMatch3GridTile::BeginPlay()
{
	Super::BeginPlay();

	Grid = Cast<AMatch3Grid>(GetOwner());

	OnClicked.AddUniqueDynamic(this, &AMatch3GridTile::TileSelected);
	OnInputTouchBegin.AddUniqueDynamic(this, &AMatch3GridTile::TileSelectedByTouch);
}

// Called every frame
void AMatch3GridTile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMatch3GridTile::TileSelected()
{
	if (Grid)
	{
		Grid->OnTileWasSelected(this);
	}
}

void AMatch3GridTile::TileSelectedByTouch(ETouchIndex::Type FingerIndex)
{
	TileSelected();
}

void AMatch3GridTile::OnMatched_Implementation(EMatch3MoveType MoveType)
{
	Grid->OnTileMatchingFinished(this);
}

void AMatch3GridTile::OnSwapMove_Implementation(AMatch3GridTile* DestinationTile, bool bMoveWillSucceed)
{
	Grid->OnSwapDisplayFinished(this);
}

void AMatch3GridTile::StartFalling(bool bUseCurrentWorldLocation)
{
	
}

void AMatch3GridTile::FinishFalling()
{
	
}

void AMatch3GridTile::TickFalling()
{

}

int32 AMatch3GridTile::GetGridAddress() const
{
	return GridAddress;
}

void AMatch3GridTile::SetGridAddress(int32 NewAddress)
{
	GridAddress = NewAddress;
}
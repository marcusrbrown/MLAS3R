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
	float fallDistance = 0.0f;

	FallingStartTime = GetWorld()->GetTimeSeconds();
	FallingStartLocation = GetActorLocation();

	// Tiles fall at a fixed rate of 120 FPS.
	GetWorldTimerManager().SetTimer(TickFallingHandle, this, &AMatch3GridTile::TickFalling, 0.001f, true);

	check(Grid);

	if (!bUseCurrentWorldLocation)
	{
		// Fall from where we are on the grid to where we're supposed to be on the grid.
		int32 yOffset = 0;
		int32 heightAboveBottom = 1;

		for (;;)
		{
			++yOffset;

			if (Grid->GetGridAddressWithOffset(GetGridAddress(), 0, -yOffset, LandingGridAddress))
			{
				if (auto TileBelow = Grid->GetTileFromGridAddress(LandingGridAddress))
				{
					if (TileBelow->TileState == EMatch3TileState::Falling)
					{
						// Keep falling on top of a falling tile.
						++heightAboveBottom;
						continue;
					}
					else if (TileBelow->TileState == EMatch3TileState::Removed)
					{
						// The tile below this one is being removed.
						continue;
					}
				}
				else
				{
					// The space below is empty, but is on the grid. We can fall through this space from above.
					continue;
				}
			}

			// This space is off the grid or contains a tile that is staying. Go back a space and start falling.
			yOffset -= heightAboveBottom;
			Grid->GetGridAddressWithOffset(GetGridAddress(), 0, -yOffset, LandingGridAddress);
			break;
		}

		fallDistance = Grid->TileSize.Y * yOffset;
		FallingEndLocation = FallingStartLocation;
		FallingEndLocation.Z -= fallDistance;
	}
	else
	{
		// Fall from where we are physically to where we are supposed to be on the grid.
		LandingGridAddress = GetGridAddress();
	}

	// TODO: Complete!
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
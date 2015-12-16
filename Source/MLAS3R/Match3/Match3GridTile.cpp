// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Match3GridTile.h"
#include "Match3Grid.h"
#include "MLAS3RGameMode.h"

// Sets default values
AMatch3GridTile::AMatch3GridTile()
: FallingStartTime(0.0f)
, TotalFallingTime(0.0f)
, FallingStartLocation(0.0f)
, FallingEndLocation(0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // Generate a default Match ID value, so that if it isn't set, the tile can only match tiles that
    // are the same class.
    MatchId = FName(*FString::Printf(TEXT("Match %s"), *GetClass()->GetFName().ToString()));
}

void AMatch3GridTile::PostInitProperties()
{
    Super::PostInitProperties();
}

// Called when the game starts or when spawned
void AMatch3GridTile::BeginPlay()
{
	Super::BeginPlay();

	Grid = Cast<AMatch3Grid>(GetOwner());

	// Mouse input.
	OnClicked.AddUniqueDynamic(this, &AMatch3GridTile::TilePress_Mouse);
	OnBeginCursorOver.AddUniqueDynamic(this, &AMatch3GridTile::TileEnter_Mouse);

	// Touch input.
	OnInputTouchBegin.AddUniqueDynamic(this, &AMatch3GridTile::TilePress);
	OnInputTouchEnter.AddUniqueDynamic(this, &AMatch3GridTile::TileEnter);
}

// Called every frame
void AMatch3GridTile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AMatch3GridTile::TileEnter_Mouse()
{
	// This is to simulate finger-swiping, so ignore if the mouse isn't clicked.
	if (APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (pc->IsInputKeyDown(EKeys::LeftMouseButton))
		{
			TileEnter(ETouchIndex::Touch1);
		}
	}
}

void AMatch3GridTile::TilePress_Mouse()
{
	TilePress(ETouchIndex::Touch1);
}

void AMatch3GridTile::TileEnter(ETouchIndex::Type FingerIndex)
{
	// TODO: marcus@HV: We could also check Grid->AreAddressesNeighbors() to enforce that two tiles must be adjacent to swap, ala Bejeweled.
	if (Grid && (Grid->SelectedTile != nullptr))
	{
		TilePress(FingerIndex);
	}
}

void AMatch3GridTile::TilePress(ETouchIndex::Type FingerIndex)
{
	if (Grid)
	{
		Grid->OnTileWasSelected(this);
	}
}

void AMatch3GridTile::OnMatched_Implementation(EMatch3MoveType MoveType)
{
	Grid->OnTileMatchingFinished(this);
}

void AMatch3GridTile::OnSwapMove_Implementation(AMatch3GridTile* DestinationTile, bool bMoveWillSucceed)
{
	Grid->OnSwapDisplayFinished(this);
}

FVector AMatch3GridTile::GetGridLocation()
{
    return Grid->GetLocationFromGridAddress(GridAddress);
}

void AMatch3GridTile::StartFalling(bool bUseCurrentWorldLocation)
{
	float fallDistance = 0.0f;

	FallingStartTime = GetWorld()->GetTimeSeconds();
    TotalFallingTime = 0.0f;
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

			if (Grid->GetGridAddressWithOffset(GetGridAddress(), 0, yOffset, LandingGridAddress))
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
			Grid->GetGridAddressWithOffset(GetGridAddress(), 0, yOffset, LandingGridAddress);
			break;
		}

        fallDistance = Grid->TileSize.Y * yOffset + (Grid->GridPadding.Y * yOffset);
		FallingEndLocation = FallingStartLocation;
		FallingEndLocation.Y -= -fallDistance;
	}
	else
	{
		// Fall from where we are physically to where we are supposed to be on the grid.
		LandingGridAddress = GetGridAddress();
	}

    TileState = EMatch3TileState::Falling;
}

void AMatch3GridTile::FinishFalling()
{
    GetWorldTimerManager().ClearTimer(TickFallingHandle);

    TileState = EMatch3TileState::Inactive;

    if (Grid)
    {
        Grid->OnTileFallingFinished(this, LandingGridAddress);
    }
}

void AMatch3GridTile::TickFalling()
{
    TotalFallingTime += 0.005f;
    float alpha = FMath::Clamp(TotalFallingTime, 0.0f, 1.0f);
    FVector FallPosition = FMath::Lerp(FallingStartLocation, FallingEndLocation, alpha);

    SetActorLocation(FallPosition);

    if (alpha >= 1.0f)
    {
        SetActorLocation(FallingEndLocation);
        FinishFalling();
    }
}

int32 AMatch3GridTile::GetGridAddress() const
{
	return GridAddress;
}

void AMatch3GridTile::SetGridAddress(int32 NewAddress)
{
	GridAddress = NewAddress;
}

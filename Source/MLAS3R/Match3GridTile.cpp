// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Match3GridTile.h"


// Sets default values
AMatch3GridTile::AMatch3GridTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMatch3GridTile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMatch3GridTile::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMatch3GridTile::OnMatched_Implementation(EMatch3MatchType MatchType)
{
	
}

void AMatch3GridTile::OnSwapMove_Implementation(AMatch3GridTile* DestinationTile, bool bMoveWillSucceed)
{
	
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
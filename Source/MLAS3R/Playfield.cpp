// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Playfield.h"
#include "TableRows.h"
#include "SplineActor.h"

namespace
{
	static const FColor DefaultBoundsColor(100, 255, 100, 255);
}

// Sets default values
APlayfield::APlayfield() : CurrentLevel(0), CurrentRow(0), PlayTime(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Setup our RootComponent
	Bounds = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Bounds->ShapeColor = DefaultBoundsColor;
	Bounds->InitBoxExtent(FVector(40.0f, 40.0f, 40.0f));
	RootComponent = Bounds;
	
	// This doesn't collide
	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void APlayfield::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayfield::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	PlayTime += DeltaTime;
	
	// Spawn the enemies and grab their spline
	if (CurrentLevel < Levels.Num())
	{
		auto row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(CurrentRow), TEXT(""));
		while (row)
		{
			if (PlayTime > (row->StartTime / 1000.0f))
			{
				UE_LOG(LogClass, Log, TEXT("Spawning %s on %s @ %f "), *row->EnemyType, *row->Spline, PlayTime);
				
				auto spline = FindSplineByName(row->Spline);
				if (spline) {
					FPlayfieldEnemyState* enemy = new FPlayfieldEnemyState();
					enemy->Spline = spline;
					enemy->Duration = row->Duration;
					enemy->DeltaTime = 0.0f;
					
					// TODO: Replace with blueprint native function
					if (row->EnemyType == FString("RedEnemy"))
					{
						enemy->Enemy = SpawnRedEnemy();
					}
					else if (row->EnemyType == FString("BlueEnemy"))
					{
						enemy->Enemy = SpawnBlueEnemy();
					}
					else if (row->EnemyType == FString("GreenEnemy"))
					{
						enemy->Enemy = SpawnGreenEnemy();
					}
					
					Enemies.Push(enemy);
				}
				
				// Next row
				row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(++CurrentRow), TEXT(""));
			}
			else
			{
				break;
			}
		}
	}
	
	// Update our enemies
	TArray<int32> doneEnemies;
	for (auto Iter(Enemies.CreateIterator()); Iter; Iter++)
	{
		auto enemyState = *Iter;
		
		// Calculate spline t value
		float length = enemyState->Spline->GetSplineLength();
		float duration = enemyState->Duration / 1000.0f;
		float delta = enemyState->DeltaTime;
		float splinePosition = delta / duration * length;
		
		// Remove enemies that have traversed the spline
		if (splinePosition > length)
		{
			doneEnemies.Push(Iter.GetIndex());
			continue;
		}
		
		// Update the enemy on the spline
		auto location = enemyState->Spline->GetLocationAtDistanceAlongSpline(splinePosition, ESplineCoordinateSpace::World);
		enemyState->Enemy->SetActorLocation(location);
		
		// Update the enemies delta time
		enemyState->DeltaTime += DeltaTime;
	}
	
	// Remove the enemies from our update list
	doneEnemies.Sort();
	for (int32 doneIndex = doneEnemies.Num()-1; doneIndex >= 0; --doneIndex)
	{
		// TODO: Signal Blueprints for a transition
		
		int32 index = doneEnemies[doneIndex];
		auto enemyState = Enemies[index];
		Enemies.RemoveAt(index);
		delete enemyState;
	}
}

#if WITH_EDITOR
void APlayfield::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	const FVector ModifiedScale = DeltaScale * ( AActor::bUsePercentageBasedScaling ? 500.0f : 5.0f );
	if ( bCtrlDown )
	{
		// CTRL+Scaling modifies trigger collision height.  This is for convenience, so that height
		// can be changed without having to use the non-uniform scaling widget (which is
		// inaccessable with spacebar widget cycling).
		FVector Extent = Bounds->GetUnscaledBoxExtent() + FVector(0, 0, ModifiedScale.X);
		Extent.Z = FMath::Max(0.0f, Extent.Z);
		Bounds->SetBoxExtent(Extent);
	}
	else
	{
		FVector Extent = Bounds->GetUnscaledBoxExtent() + FVector(ModifiedScale.X, ModifiedScale.Y, ModifiedScale.Z);
		Extent.X = FMath::Max(0.0f, Extent.X);
		Extent.Y = FMath::Max(0.0f, Extent.Y);
		Extent.Z = FMath::Max(0.0f, Extent.Z);
		Bounds->SetBoxExtent(Extent);
	}
}
#endif

AActor* APlayfield::SpawnRedEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	return world->SpawnActor(RedEnemy);
}

AActor* APlayfield::SpawnBlueEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	return world->SpawnActor(BlueEnemy);
}

AActor* APlayfield::SpawnGreenEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	return world->SpawnActor(GreenEnemy);
}

USplineComponent* APlayfield::FindSplineByName(FString name)
{
	for (TActorIterator<ASplineActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == name) return ActorItr->Spline;
	}
	return nullptr;
}

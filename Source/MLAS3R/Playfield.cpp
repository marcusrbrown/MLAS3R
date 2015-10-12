// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Playfield.h"
#include "TableRows.h"
#include "SplineActor.h"

namespace
{
	static const FColor DefaultBoundsColor(100, 255, 100, 255);
	static const FVector DefaultSpawnLocation(0.0f, 2000.0f, 0.0f);
	static const FRotator DefaultSpawnRotation(0.0f, 90.0f, 0.0f);
}

// Sets default values
APlayfield::APlayfield() : CurrentLevel(0), CurrentRow(0), PlayTime(0.0f), SpeedMultiplier(1.0f)
{
	// Set this actor to call Tick() every frame.  You can tu  rn this off to improve performance if you don't need it.
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
	
	PlayTime += DeltaTime * SpeedMultiplier;
	
	// Spawn the enemies and grab their spline
	if (CurrentLevel < Levels.Num())
	{
		// Grab a row, if the row doesn't exist, loop around and increment our speed mulitplier
		auto row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(CurrentRow), TEXT(""), false);
		if (row == nullptr)
		{
			CurrentRow = 0;
			SpeedMultiplier += 0.25;
			PlayTime = -5.0f; // 5 second delay
			row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(CurrentRow), TEXT(""), false);
		}
		
		// Spawn all enemies that should be in existance at this time point
		while (row != nullptr)
		{
			if (PlayTime > (row->StartTime / 1000.0f)) // Seconds
			{
				SpawnEnemyFromTableRow(*row);
				row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(++CurrentRow), TEXT(""), false);
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
		auto& enemyState = *Iter;
		
		// Check to ensure our enemy was not removed
		if(!IsValid(enemyState.Enemy))
		{
			doneEnemies.Push(Iter.GetIndex());
			continue;
		}
		
		// Calculate spline t value
		float length = enemyState.IntroSpline->GetSplineLength();
		float splinePosition = enemyState.Speed * enemyState.DeltaTime;
		float clampedPosition = FMath::Clamp<float>(splinePosition, 0.0f, length);
		float bulletTime = clampedPosition / length;
		
		// Update the enemy on the spline
		auto location = enemyState.IntroSpline->GetLocationAtDistanceAlongSpline(clampedPosition, ESplineCoordinateSpace::World);
		enemyState.Enemy->SetActorLocation(location);
		
		// Check to see if we need to fire
		while (enemyState.IntroBulletIndex < enemyState.IntroBullets.Num())
		{
			float t = enemyState.IntroBullets[enemyState.IntroBulletIndex];
			if (bulletTime >= t)
			{
				UE_LOG(LogTemp, Log, TEXT("SPAWNING BULLET %d @ %f"), enemyState.IntroBulletIndex, t);
				auto bulletLocation = enemyState.IntroSpline->GetLocationAtDistanceAlongSpline(t * length, ESplineCoordinateSpace::World);
				enemyState.IntroBulletIndex++;
				SpawnEnemyBulletAtLocation(enemyState.Type, bulletLocation);
			}
			else
			{
				break;
			}
		}
		
		// Update the enemies delta time
		enemyState.DeltaTime += DeltaTime;
		
		// Remove enemies that have traversed the spline
		if (splinePosition > length) {
			enemyState.Enemy->Destroy();
			doneEnemies.Push(Iter.GetIndex());
		}
	}
	
	// Remove the enemies from our update list
	doneEnemies.Sort();
	for (int32 doneIndex = doneEnemies.Num()-1; doneIndex >= 0; doneIndex--)
	{
		// TODO: Signal Blueprints for a transition
		int32 index = doneEnemies[doneIndex];
		Enemies.RemoveAt(index);
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
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return world->SpawnActor(RedEnemy, &DefaultSpawnLocation, &DefaultSpawnRotation, spawnParams);
}

AActor* APlayfield::SpawnBlueEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return world->SpawnActor(BlueEnemy, &DefaultSpawnLocation, &DefaultSpawnRotation, spawnParams);
}

AActor* APlayfield::SpawnGreenEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return world->SpawnActor(GreenEnemy, &DefaultSpawnLocation, &DefaultSpawnRotation, spawnParams);
}

AActor* APlayfield::SpawnEnemyFromTableRow(const FPlayfieldSpawnTableRow& row)
{
	auto introSpline = FindSplineByName(row.IntroSpline);
	if (introSpline == nullptr) return nullptr;
	
	// Store off our row into the enemy state
	FPlayfieldEnemyData enemy;
	enemy.Type = row.EnemyType;
	enemy.State = EPlayfieldEnemyState::Intro;
	enemy.DeltaTime = 0.0f;
	enemy.Speed = row.Speed * SpeedMultiplier;
	enemy.GridAddress = row.GridAddress;
	
	enemy.IntroSpline = introSpline;
	ParseBulletString(row.IntroBullets, enemy.IntroBullets);
	enemy.IntroBulletIndex = 0;
	
	enemy.AttackSpline = FindSplineByName(row.AttackSpline);
	ParseBulletString(row.AttackBullets, enemy.AttackBullets);
	enemy.AttackBulletIndex = 0;
	
	// TODO: Replace with blueprint native function
	if (enemy.Type == FString("RedEnemy"))
	{
		enemy.Enemy = SpawnRedEnemy();
	}
	else if (enemy.Type== FString("BlueEnemy"))
	{
		enemy.Enemy = SpawnBlueEnemy();
	}
	else if (enemy.Type == FString("GreenEnemy"))
	{
		enemy.Enemy = SpawnGreenEnemy();
	}
	
	// Add the enemy to our active tick list
	Enemies.Push(enemy);
	
	
	return enemy.Enemy;
}

AActor* APlayfield::SpawnEnemyBulletAtLocation(const FString& type, const FVector& location)
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	if (type == FString("RedEnemy"))
	{
		return world->SpawnActor(RedBullet, &location, &DefaultSpawnRotation, spawnParams);
	}
	else if (type == FString("BlueEnemy"))
	{
		return world->SpawnActor(BlueBullet, &location, &DefaultSpawnRotation, spawnParams);
	}
	else if (type == FString("GreenEnemy"))
	{
		return world->SpawnActor(GreenBullet, &location, &DefaultSpawnRotation, spawnParams);
	}
	
	return nullptr;
}

USplineComponent* APlayfield::FindSplineByName(FString name)
{
	for (TActorIterator<ASplineActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == name) return ActorItr->Spline;
		if (ActorItr->ActorHasTag(FName(*name))) return ActorItr->Spline;
	}
	return nullptr;
}

void APlayfield::ParseBulletString(const FString& bulletString, TArray<float>& OutArray)
{
	TArray<FString> bulletTimeValues;
	bulletString.ParseIntoArrayWS(bulletTimeValues, TEXT(","), true);
	for (auto& value : bulletTimeValues)
	{
		OutArray.Push(FCString::Atof(*value));
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Playfield.h"
#include "TableRows.h"
#include "Enemy.h"

namespace
{
	static const FColor DefaultBoundsColor(100, 255, 100, 255);
	static const FVector DefaultSpawnLocation(0.0f, 2000.0f, 0.0f);
	static const FRotator DefaultSpawnRotation(0.0f, 90.0f, 0.0f);
}

// Sets default values
APlayfield::APlayfield() : CurrentLevel(0), CurrentRow(0), PlayTime(0.0f), 
    SpeedMultiplier(1.0f), PlayerIsDead(false), WaitingForWaveClear(false), 
    Attacking(false), LoopWaveStart(0), TimeDilation(1.0f), SuppressFire(false)
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
	
	// Init sane grid defaults
	Grid.Rows = 4;
	Grid.Columns = 10;
	Grid.CellExtent = FVector(450.0f, 450.0f, 0.0f);
	Grid.Padding = FVector(10.0f, 10.0f, 0.0f);
	Grid.Offset = FVector(0.0f, 600.0f, 0.0f);
	Grid.Pivot = FVector(0.0f, 0.0f, 0.0f);
}

// Called when the game starts or when spawned
void APlayfield::BeginPlay()
{
	Super::BeginPlay();
	
	// Calculate the Grid
	Grid.Offset += GetActorLocation() - Bounds->GetScaledBoxExtent();
	Grid.Width = Grid.CellExtent.X * Grid.Columns + (Grid.Padding.X * Grid.Columns);
	Grid.Height = Grid.CellExtent.Y * Grid.Rows + (Grid.Padding.Y * Grid.Rows);
	Grid.Offset.X += ((Bounds->GetScaledBoxExtent().X * 2) - Grid.Width) * 0.5f;
	Grid.LerpAlpha = 0.5f;
	Grid.LerpAlphaDirection = 1.0f;
	Grid.LerpDuration = 5.0f;
}

// Called every frame
void APlayfield::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

    // Allow for spline time dilation
    DeltaTime = DeltaTime * TimeDilation;
	
	// Update the grid pivot
	{
		Grid.LerpAlpha += Grid.LerpAlphaDirection * DeltaTime * SpeedMultiplier;
		float alpha = Grid.LerpAlpha / Grid.LerpDuration;
		alpha = FMath::Clamp(alpha, 0.0f, 1.0f);
		
		if (alpha >= 1.0f) Grid.LerpAlphaDirection = -1.0f;
		if (alpha <= 0.0f) Grid.LerpAlphaDirection = 1.0f;
		
		float start = GetActorLocation().X + Grid.Padding.X + Grid.CellExtent.X;
		float end = (GetActorLocation() + (Bounds->GetScaledBoxExtent() * 0.5f)).X - Grid.Padding.X - Grid.CellExtent.X;
		float distance = end - start;
		Grid.Pivot = FMath::Lerp(FVector(-distance, 0.0f, 0.0f), FVector(distance, 0.0f, 0.0f), alpha);
	}
	
	// Spawn the enemies and grab their spline
	if (CurrentLevel < Levels.Num())
	{
		if (WaitingForWaveClear && Enemies.Num() == 0)
		{
			Attacking = false;
			WaitingForWaveClear = false;
			PlayTime = 0.0f;
		}
		
		if (!WaitingForWaveClear && !PlayerIsDead)
		{
			PlayTime += DeltaTime * SpeedMultiplier;
			
			// Grab a row, if the row doesn't exist, loop around and increment our speed mulitplier (once all enemies are dead)
			auto row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(CurrentRow), TEXT(""), false);
			
			if (!WaitingForWaveClear)
			{
				// Restart
				if (row == nullptr && Enemies.Num() == 0) // TODO: Can Spawn Helper Method
				{
					CurrentRow = LoopWaveStart;
					SpeedMultiplier += 0.25;
                    PlayTime = 0.0f;
					row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(CurrentRow), TEXT(""), false);
				}
				
				// Spawn all enemies that should be in existance at this time point
				while (row != nullptr)
				{
					if (PlayTime > (row->StartTime / 1000.0f)) // Seconds
					{
						SpawnEnemyFromTableRow(*row);
						row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(++CurrentRow), TEXT(""), false);
						
						// Enemy Wave Wait
						if (row != nullptr && row->EnemyType == FString("Wait"))
						{
							WaitingForWaveClear = true;
							row = ((UDataTable*)Levels[CurrentLevel])->FindRow<FPlayfieldSpawnTableRow>(*FString::FromInt(++CurrentRow), TEXT(""), false);
							break;
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}
	
	// Update our enemies
	bool readyForAttack = true;
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
		
		switch (enemyState.State)
		{
			case EPlayfieldEnemyState::Intro:
			{
				// Calculate spline t value
				float length = enemyState.IntroSpline->GetSplineLength();
				float splinePosition = enemyState.Speed * enemyState.DeltaTime;
				float clampedPosition = FMath::Clamp(splinePosition, 0.0f, length);
				float bulletTime = clampedPosition / length;
				
				// Update the enemy on the spline
                enemyState.Location = enemyState.IntroSpline->GetLocationAtDistanceAlongSpline(clampedPosition, ESplineCoordinateSpace::World);
				enemyState.Enemy->SetActorLocation(enemyState.Location);
				
				// Check to see if we need to fire
				while (enemyState.IntroTriggerIndex < enemyState.IntroTriggers.Num())
				{
					auto trigger = enemyState.IntroTriggers[enemyState.IntroTriggerIndex];
					float t = trigger.Time;
					if (bulletTime >= t && trigger.Action == ESplineTriggerAction::Fire)
					{
						enemyState.IntroTriggerIndex++;
						
						if (trigger.bActionValue)
						{
							enemyState.FireEnabled = true;
							enemyState.FireDelayAlpha = enemyState.FirstShotDelay;
						}
						else
						{
							enemyState.FireEnabled = false;
						}
					}
					else
					{
						break;
					}
				}
				
				// Check to see if we need to fire
				if (enemyState.FireEnabled)
				{
					enemyState.FireDelayAlpha -= DeltaTime * SpeedMultiplier;
					
					if (enemyState.FireDelayAlpha <= 0.0f)
					{
						enemyState.FireDelayAlpha = enemyState.FireDelay;
						
                        if (!SuppressFire)
                        {
                            auto bulletLocation = enemyState.IntroSpline->GetLocationAtDistanceAlongSpline(clampedPosition, ESplineCoordinateSpace::World);
                            SpawnEnemyBulletAtLocation(enemyState.Type, bulletLocation);
                        }
					}
				}
				
				// Update the enemies delta time
				enemyState.DeltaTime += DeltaTime;
				
				// Move the enemies to the grid
				if (splinePosition > length)
                {
                    if (enemyState.GridAddress < 0)
                    {
                        enemyState.Enemy->Destroy();
                        doneEnemies.Push(Iter.GetIndex());
                    }
                    else
                    {
                        enemyState.State = EPlayfieldEnemyState::ToFormation;
                        enemyState.LerpAlpha = 0.0f;
                        enemyState.LerpStart = enemyState.Enemy->GetActorLocation();
                    }
				}
			} break;
				
			case EPlayfieldEnemyState::ToFormation:
			{
                FVector target = GetGridLocationFromAddress(enemyState.GridAddress);
                float length = (target - enemyState.LerpStart).Size();

                float distance = enemyState.Speed * enemyState.LerpAlpha;
                distance = FMath::Clamp(distance, 0.0f, length);
                float alpha = distance / length;
				
                enemyState.Location = FMath::Lerp(enemyState.LerpStart, target, alpha);
				enemyState.Enemy->SetActorLocation(enemyState.Location);
                enemyState.LerpAlpha += DeltaTime;
				
                if (alpha >= 1.0f)
				{
					enemyState.State = EPlayfieldEnemyState::Formation;
					enemyState.AttackAlpha = 0.0f;
				}
			} break;
				
			case EPlayfieldEnemyState::Formation:
			{
                enemyState.Location = GetGridLocationFromAddress(enemyState.GridAddress);
				enemyState.Enemy->SetActorLocation(enemyState.Location);
				
				if (Attacking)
				{
					enemyState.AttackAlpha += DeltaTime * SpeedMultiplier;
					if (enemyState.AttackAlpha >= enemyState.AttackTime)
					{
						if (enemyState.AttackSpline != nullptr)
						{
							enemyState.State = EPlayfieldEnemyState::ToAttack;
							enemyState.LerpAlpha = 0.0f;
                            enemyState.LerpStart = enemyState.Enemy->GetActorLocation();
						}
					}
				}
			} break;
				
			case EPlayfieldEnemyState::ToAttack:
			{
                FVector target = enemyState.AttackSpline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
                float length = (target - enemyState.LerpStart).Size();

                float distance = enemyState.Speed * enemyState.LerpAlpha;
                distance = FMath::Clamp(distance, 0.0f, length);
                float alpha = distance / length;

                enemyState.Location = FMath::Lerp(enemyState.LerpStart, target, alpha);
                enemyState.Enemy->SetActorLocation(enemyState.Location);
                enemyState.LerpAlpha += DeltaTime;
				
                if (alpha >= 1.0f)
				{
					enemyState.State = EPlayfieldEnemyState::Attack;
					enemyState.DeltaTime = 0.0f;
					enemyState.AttackTriggerIndex = 0;
				}
			} break;
				
			case EPlayfieldEnemyState::Attack:
			{
				// Calculate spline t value
				float length = enemyState.AttackSpline->GetSplineLength();
				float splinePosition = enemyState.Speed * enemyState.DeltaTime;
				float clampedPosition = FMath::Clamp(splinePosition, 0.0f, length);
				float bulletTime = clampedPosition / length;
				
				// Update the enemy on the spline
                enemyState.Location = enemyState.AttackSpline->GetLocationAtDistanceAlongSpline(clampedPosition, ESplineCoordinateSpace::World);
				enemyState.Enemy->SetActorLocation(enemyState.Location);
				
				// Check to see if we need to fire
				while (enemyState.AttackTriggerIndex < enemyState.AttackTriggers.Num())
				{
					auto trigger = enemyState.AttackTriggers[enemyState.AttackTriggerIndex];
					float t = trigger.Time;
					if (bulletTime >= t && trigger.Action == ESplineTriggerAction::Fire)
					{
						enemyState.AttackTriggerIndex++;
						
						if (trigger.bActionValue)
						{
							enemyState.FireEnabled = true;
							enemyState.FireDelayAlpha = enemyState.FirstShotDelay;
						}
						else
						{
							enemyState.FireEnabled = false;
						}
					}
					else
					{
						break;
					}
				}
				
				// Check to see if we need to fire
				if (enemyState.FireEnabled)
				{
					enemyState.FireDelayAlpha -= DeltaTime * SpeedMultiplier;
					
					if (enemyState.FireDelayAlpha <= 0.0f)
					{
						enemyState.FireDelayAlpha = enemyState.FireDelay;
						
                        if (!SuppressFire)
                        {
                            auto bulletLocation = enemyState.AttackSpline->GetLocationAtDistanceAlongSpline(clampedPosition, ESplineCoordinateSpace::World);
                            SpawnEnemyBulletAtLocation(enemyState.Type, bulletLocation);
                        }
					}
				}
				
				// Update the enemies delta time
				enemyState.DeltaTime += DeltaTime;
				
				// Move the enemies to the grid
				if (splinePosition > length) {
					enemyState.State = EPlayfieldEnemyState::BackToFormation;
				}
			} break;
				
			case EPlayfieldEnemyState::BackToFormation:
			{
				FVector target = GetGridLocationFromAddress(enemyState.GridAddress);
				FVector bounds = GetActorLocation() - Bounds->GetScaledBoxExtent() - Grid.CellExtent.Y;
				target += FVector(0.0f, bounds.Y, 0.0f);
                enemyState.Location = target;
				enemyState.Enemy->SetActorLocation(enemyState.Location);
				
				enemyState.State = EPlayfieldEnemyState::ToFormation;
				enemyState.LerpAlpha = 0.0f;
                enemyState.LerpStart = enemyState.Enemy->GetActorLocation();
			} break;
		}
		
		
		// Calculate Global Attacking State
		if (enemyState.State != EPlayfieldEnemyState::Formation)
		{
			readyForAttack = false;
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
	
	// Start the attack
	if (readyForAttack && WaitingForWaveClear)
	{
		Attacking = true;
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

AEnemy* APlayfield::SpawnRedEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto actor = Cast<AEnemy>(world->SpawnActor(RedEnemy, &DefaultSpawnLocation, &DefaultSpawnRotation, spawnParams));
    if (actor != nullptr)
    {
        actor->Color = EnemyColor::Red;
    }
    return actor;
}

AEnemy* APlayfield::SpawnBlueEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto actor = Cast<AEnemy>(world->SpawnActor(BlueEnemy, &DefaultSpawnLocation, &DefaultSpawnRotation, spawnParams));
    if (actor != nullptr)
    {
        actor->Color = EnemyColor::Blue;
    }
    return actor;
}

AEnemy* APlayfield::SpawnGreenEnemy()
{
	UWorld* world = GetWorld();
	if (!world) return nullptr;
	
	FActorSpawnParameters spawnParams;
	spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto actor = Cast<AEnemy>(world->SpawnActor(GreenEnemy, &DefaultSpawnLocation, &DefaultSpawnRotation, spawnParams));
    if (actor != nullptr)
    {
        actor->Color = EnemyColor::Green;
    }
    return actor;
}

AEnemy* APlayfield::SpawnYellowEnemy()
{
    UWorld* world = GetWorld();
    if (!world) return nullptr;

    FActorSpawnParameters spawnParams;
    spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto actor = Cast<AEnemy>(world->SpawnActor(YellowEnemy, &DefaultSpawnLocation, &DefaultSpawnRotation, spawnParams));
    if (actor != nullptr)
    {
        actor->Color = EnemyColor::Yellow;
    }
    return actor;
}

AEnemy* APlayfield::SpawnEnemyFromTableRow(const FPlayfieldSpawnTableRow& row)
{
	if (!FindSplineByName(row.IntroSpline)) return nullptr;
	
	// Store off our row into the enemy state
	FPlayfieldEnemyData enemy;
	enemy.Type = row.EnemyType;
	enemy.State = EPlayfieldEnemyState::Intro;
	enemy.DeltaTime = 0.0f;
	enemy.Speed = row.Speed * SpeedMultiplier;
	enemy.GridAddress = row.GridAddress;
	
	enemy.FireEnabled = false;
    enemy.FirstShotDelay = row.FirstShotDelay / 1000.0f;
	enemy.FireDelay = row.FireDelay / 1000.0f;
	enemy.FireDelayAlpha = 0.0f;

	enemy.IntroSpline = nullptr;
	auto introSpline = FindSplineByName(row.IntroSpline);
	if (introSpline)
	{
		enemy.IntroSpline = introSpline->Spline;
		enemy.IntroTriggers = introSpline->Triggers;
		enemy.IntroTriggerIndex = 0;
	}
	
	enemy.AttackSpline = nullptr;
	auto attackSpline = FindSplineByName(row.AttackSpline);
	if (attackSpline)
	{
		enemy.AttackSpline = attackSpline->Spline;
		enemy.AttackTriggers = attackSpline->Triggers;
	}
	enemy.AttackTriggerIndex = 0;
	
	enemy.AttackTime = row.AttackTime / 1000.0f;
	enemy.AttackAlpha = 0.0f;
	
	enemy.LerpAlpha = 0.0f;
	enemy.LerpDuration = enemy.Speed / 1000.0f;
	
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
    else if (enemy.Type == FString("YellowEnemy"))
    {
        enemy.Enemy = SpawnYellowEnemy();
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
    else if (type == FString("YellowEnemy"))
    {
        return world->SpawnActor(YellowBullet, &location, &DefaultSpawnRotation, spawnParams);
    }
	
	return nullptr;
}

void APlayfield::SetPlayerIsDead(bool value)
{
	PlayerIsDead = value;
}

void APlayfield::ToggleGrid(bool bEnabled)
{
    for (auto enemyState : Enemies)
    {
        if (IsValid(enemyState.Enemy))
        {
            if (bEnabled) enemyState.Enemy->SetActorHiddenInGame(false);
            enemyState.Enemy->CaptureLocation = enemyState.Location;
            enemyState.Enemy->SetActorEnableCollision(bEnabled);
        }
    }
}

void APlayfield::ResetState()
{
    CurrentLevel = 0;

    CurrentRow = 0;

    PlayTime = 0.0f;

    SpeedMultiplier = 1.0f;

    PlayerIsDead = false;

    Grid.LerpAlpha = 0.5f;
    Grid.LerpAlphaDirection = 1.0f;
    Grid.LerpDuration = 5.0f;

    WaitingForWaveClear = false;

    Attacking = false;

    for (auto enemyState : Enemies)
    {
        if (IsValid(enemyState.Enemy))
        {
            enemyState.Enemy->Destroy();
        }
    }

    Enemies.Reset();
}

ASplineActor* APlayfield::FindSplineByName(FString name)
{
	for (TActorIterator<ASplineActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == name) return *ActorItr;
		if (ActorItr->ActorHasTag(FName(*name))) return *ActorItr;
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

FVector APlayfield::GetGridLocationFromAddress(int32 address)
{
	check(address >= 0 && address < Grid.Rows * Grid.Columns);
	
	int32 column = address % Grid.Columns;
	int32 row  = address / Grid.Columns;
	
	// Calculate x,y in grid
	float x = Grid.Pivot.X + Grid.Offset.X + (column * Grid.CellExtent.X + (Grid.Padding.X * column));
	float y = Grid.Pivot.Y + Grid.Offset.Y + (row * Grid.CellExtent.Y + (Grid.Padding.Y * row));
	
	// Center in cell
	x += Grid.CellExtent.X * 0.5f;
	y += Grid.CellExtent.Y * 0.5f;
	
	return FVector(x, y, 0.0f);
}

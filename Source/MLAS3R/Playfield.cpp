// Fill out your copyright notice in the Description page of Project Settings.

#include "MLAS3R.h"
#include "Playfield.h"

namespace
{
   static const FColor DefaultBoundsColor(100, 255, 100, 255);
}

// Sets default values
APlayfield::APlayfield()
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

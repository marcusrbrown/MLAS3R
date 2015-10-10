#pragma once

#include "Engine/DataTable.h"
#include "TableRows.generated.h"

USTRUCT(BlueprintType)
struct MLAS3R_API FPlayfieldSpawnTableRow : public FTableRowBase
{
   GENERATED_USTRUCT_BODY()
   
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
   int32 Delay;
   
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
   int32 EnemyType;
   
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
   FString Spline;
};

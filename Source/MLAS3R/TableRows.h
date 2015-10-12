#pragma once

#include "Engine/DataTable.h"
#include "TableRows.generated.h"

USTRUCT(BlueprintType)
struct MLAS3R_API FPlayfieldSpawnTableRow : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	FString EnemyType;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	int32 StartTime;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	float Speed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	int32 GridAddress;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	FString IntroSpline;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	FString IntroBullets;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	FString AttackSpline;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enemy)
	FString AttackBullets;
};

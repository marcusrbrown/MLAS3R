// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "MLAS3RPlayerController.h"
#include "Playfield.h"
#include "Match3Grid.h"
#include "MLAS3RGameMode.generated.h"

/** Reward granted during Match 3. */
// TODO: marcus@HV: This may be a placeholder, or it may be extended. Dunno yet.
USTRUCT()
struct FMatch3Reward
{
	GENERATED_USTRUCT_BODY()

	/** Reward triggers at this interval. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ScoreInterval;

	/** Reward grants this much time upon triggering. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeAwarded;
};

/** Game State **/
UENUM(Blueprintable, Category = "State")
enum class EGameState : uint8
{
	None,
	Start,
	Play,
	Match3,
	GameOver
};

/**
 *  Delegates
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGameStateChangedDelegate, EGameState, ActiveState, EGameState, PreviousState);

/**
 * 
 */
UCLASS(Blueprintable)
class MLAS3R_API AMLAS3RGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMLAS3RGameMode();
	
	void BeginPlay() override;
	
	void Tick(float DeltaSeconds) override;

    /** Return a reference to the global AMLAS3RPlayerController instance. */
    UFUNCTION(BlueprintCallable, Category = "References")
    class AMLAS3RPlayerController* GetPlayerController();

    /** Return a reference to the global APlayfield instance. */
    UFUNCTION(BlueprintCallable, Category = "References")
    class APlayfield* GetPlayfield();

    /** Return a reference to the global AMatch3Grid instance. */
    UFUNCTION(BlueprintCallable, Category = "References")
    class AMatch3Grid* GetMatch3Grid();

	/** The speed at which tiles fall into place. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match 3 Game")
	float TileFallSpeed;
	
	/** Game State **/
	UFUNCTION(BlueprintCallable, Category = "State")
	void RequestGameState(EGameState State);
	
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EGameState ActiveState;
	
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EGameState PreviousState;
	
	/** Event Delegate **/
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameStateChangedDelegate OnStateChanged;
	
private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References", meta = (AllowPrivateAccess = "true"))
    AMLAS3RPlayerController* PlayerController;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References", meta = (AllowPrivateAccess = "true"))
    APlayfield* Playfield;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References", meta = (AllowPrivateAccess = "true"))
    AMatch3Grid* Match3Grid;

	EGameState PendingState;
};

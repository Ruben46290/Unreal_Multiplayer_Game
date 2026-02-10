// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeRemainingChanged, float, NewTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, float, NewScore);

UCLASS()
class MULTIPLAYER_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AMyGameState();



	// * * * * * * * * * * Level Timer * * * * * * * * * * \\

	// How Long Should The Level Be - In Seconds
	UPROPERTY()
	float LevelDuration = 70.0f;

	// Time Left In The Level
	UPROPERTY(ReplicatedUsing = OnRep_TimeRemaning)
	float TimeRemaining;

	// Replicaton Event For Time Remaining
	UFUNCTION()
	void OnRep_TimeRemaning();

	// Event Dispatcher For Updating UI When Time Remaining Is Changed
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnTimeRemainingChanged OnTimeRemainingChanged;

	// How Fast Should The Level Timer Tick
	UPROPERTY()
	float TimerTickSpeed = 1.0f;

	// Timer For Ticking Down Level Time - Ticks At TimerTickSpeed 
	FTimerHandle LevelTimer;

	UFUNCTION()
	void LevelTimerTick();

	UFUNCTION()
	void StartLevelTimer();

	UFUNCTION()
	void StopLevelTimer();


	// * * * * * * * * * * Score * * * * * * * * * * \\

	UFUNCTION()
	void AddScore(float ScoreToAdd);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentScore)
	float CurrentScore = 0.0f;

	UFUNCTION()
	void OnRep_CurrentScore();

	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnScoreChanged OnScoreChanged;
};

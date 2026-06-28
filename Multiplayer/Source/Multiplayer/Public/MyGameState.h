// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeRemainingChanged, float, NewTime);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, float, NewScore);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreMilestoneReached, int32, CurrentStars);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelComplete);

UCLASS()
class MULTIPLAYER_API AMyGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AMyGameState();

	virtual void BeginPlay() override;

	// * * * * * * * * * * Level Timer * * * * * * * * * * \\

	// How Long Should The Level Be - In Seconds
	UPROPERTY()
	float LevelDuration = 90.0f;

	// Time Left In The Level
	UPROPERTY(ReplicatedUsing = OnRep_TimeRemaning)
	float TimeRemaining;

	// Replicaton Event For Time Remaining
	UFUNCTION()
	void OnRep_TimeRemaning();

	// Event Dispatcher For Updating UI When Time Remaining Is Changed
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnTimeRemainingChanged OnTimeRemainingChanged;

	// Event Dispatcher For When Level Time Runs Out
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnLevelComplete OnLevelComplete;

	// How Fast Should The Level Timer Tick
	UPROPERTY()
	float TimerTickSpeed = 1.0f;

	// Timer For Ticking Down Level Time - Ticks At TimerTickSpeed 
	FTimerHandle LevelTimer;

	UFUNCTION()
	void LevelTimerTick();

	UFUNCTION()
	void StartLevelTimer(float Duration = 60.0f);

	UFUNCTION()
	void StopLevelTimer();

	UPROPERTY(ReplicatedUsing = OnRep_LevelComplete)
	bool LevelComplete = false;

	UFUNCTION()
	void OnRep_LevelComplete();

	// * * * * * * * * * * Score * * * * * * * * * * \\

	UFUNCTION()
	void AddScore(float ScoreToAdd);

	UPROPERTY(ReplicatedUsing = OnRep_CurrentScore)
	float CurrentScore = 0.0f;

	UFUNCTION()
	void OnRep_CurrentScore();

	// Event Dispatcher For Updating UI When Score Is Changed - Bound On GameHUD
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnScoreChanged OnScoreChanged;

	// Event Dispatcher For When Score Milestone Is Reached - Bound On GameHUD
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnScoreMilestoneReached OnScoreMilestoneReached;

	// Loaded From Level Settings Actor
	TArray<float> ScoreMilestones;

	// Track The Last Milestone Index 
	UPROPERTY(ReplicatedUsing = OnRep_MilestoneIndex)
	int32 LastMilestoneIndex = -1;

	UFUNCTION()
	void OnRep_MilestoneIndex();

	// Check If Any Milestones Have Been Hit And Broadcast Event If They Have
	void CheckScoreMilestones();


	// * * * * * * * * * * * Next Level * * * * * * * * * \\

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<UWorld> NextLevel;

	UPROPERTY(BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<UWorld> LobbyLevel;

	UFUNCTION(BlueprintCallable, Category = "Level")
	void LoadNextLevel() const;

	UFUNCTION()
	void ReplayLevel() const;

	UFUNCTION()
	void GoToLobby() const;
};

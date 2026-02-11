// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "Net/UnrealNetwork.h"



AMyGameState::AMyGameState()
{
}





void AMyGameState::LevelTimerTick()
{
	// Decrease Level Time
	TimeRemaining -= TimerTickSpeed;

	// Call Event Dispatcher To Update UI
	OnTimeRemainingChanged.Broadcast(TimeRemaining);

	// If Level Is Done
	if (TimeRemaining <= 0) {

		// Stop Timer
		StopLevelTimer();
		
		// Change A Bool Var To Call OnRep_LevelComplete For Clients To Get The UI Aswell
		LevelComplete = true;

		// Broadcast Level Complete Dispatcher
		OnLevelComplete.Broadcast();
	}

}

void AMyGameState::OnRep_TimeRemaning()
{
	// Call Event Dispatcher To Update UI
	OnTimeRemainingChanged.Broadcast(TimeRemaining);
}

void AMyGameState::OnRep_LevelComplete()
{
	if (LevelComplete)
	{
		OnLevelComplete.Broadcast();
	}
}

void AMyGameState::StartLevelTimer()
{
	// Only Run On Server
	if (HasAuthority()) {

		// If Level Timer Isn't Active
		if (!GetWorld()->GetTimerManager().IsTimerActive(LevelTimer)) {

			// Set Time Remaning To Total Level Duration
			TimeRemaining = LevelDuration;

			// Start The Level Timer
			GetWorldTimerManager().SetTimer(LevelTimer, this, &AMyGameState::LevelTimerTick, TimerTickSpeed, true);

		}

	}
}

void AMyGameState::StopLevelTimer()
{
	// Only Run On Server
	if (HasAuthority()) {

		// Stop Level Timer
		GetWorldTimerManager().ClearTimer(LevelTimer);
	}
}

// * * * * * * * * * * Score * * * * * * * * * * \\

void AMyGameState::AddScore(float ScoreToAdd)
{
	// Only Run On Server
	if (!HasAuthority()) { return; }

	// Add New Score To Total Score
	CurrentScore += ScoreToAdd;

	// Call Event Dispatcher For Updating Score UI
	OnScoreChanged.Broadcast(CurrentScore);
}


void AMyGameState::OnRep_CurrentScore()
{
	// Runs On Clients

	// Call Event Dispatcher For Updating Score UI
	OnScoreChanged.Broadcast(CurrentScore);
}



void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, TimeRemaining);
	DOREPLIFETIME(AMyGameState, CurrentScore);
	DOREPLIFETIME(AMyGameState, LevelComplete);
}

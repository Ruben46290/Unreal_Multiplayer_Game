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

	// Debugging Print Until UI Is Made
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, TimerTickSpeed, FColor::Cyan,
			FString::Printf(TEXT("Level Time Remaining: %.1f seconds"), TimeRemaining));
	}

	// If Level Is Done
	if (TimeRemaining <= 0) {

		// Print Until Game Finish Logic Is Done
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, TEXT("Level Timer Finished")); }

		// Stop Timer
		StopLevelTimer();

	}

}

void AMyGameState::OnRep_TimeRemaning()
{
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, TimerTickSpeed, FColor::Cyan,
			FString::Printf(TEXT("Time Remaining OnRep Called ")));
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



void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, TimeRemaining);
}
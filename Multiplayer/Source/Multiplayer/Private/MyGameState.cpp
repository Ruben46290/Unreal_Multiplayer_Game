// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameState.h"
#include "Net/UnrealNetwork.h"
#include "LevelSettings.h"
#include <Kismet/GameplayStatics.h>


AMyGameState::AMyGameState()
{
}

void AMyGameState::BeginPlay()
{
	Super::BeginPlay();

	// Only server needs to track milestones
	if (HasAuthority())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelSettings::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0)
		{
			// Cast To Level Settings
			ALevelSettings* LoadedLevelSettings = Cast<ALevelSettings>(FoundActors[0]);

			// Load Score Milestones From Level Settings Actor
			ScoreMilestones = LoadedLevelSettings->ScoreMilestones;

			// Load & Save Next Level From Level Settings Actor
			NextLevel = LoadedLevelSettings->NextLevel;

			// Load & Save Lobby Level From Level Settings Actor
			LobbyLevel = LoadedLevelSettings->LobbyLevel;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("GameState: No LevelSettings actor found in level!"));
		}
	}
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

void AMyGameState::StartLevelTimer(float Duration)
{
	// Only Run On Server
	if (HasAuthority()) {

		// Store the duration
		LevelDuration = Duration;

		//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GameState Start Level Timer")); }

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

	// Check If Any Milestones Have Been Hit
	CheckScoreMilestones();
}


void AMyGameState::OnRep_CurrentScore()
{
	// Runs On Clients

	// Call Event Dispatcher For Updating Score UI
	OnScoreChanged.Broadcast(CurrentScore);

	// Check If Any Milestones Have Been Hit
	CheckScoreMilestones();
}

void AMyGameState::CheckScoreMilestones()
{
	// For Each Milestone
	for (int32 i = 0; i < ScoreMilestones.Num(); i++) {

		// Skip If Milestone Has Already Been Hit
		if (i <= LastMilestoneIndex) { continue; }

		if (CurrentScore >= ScoreMilestones[i]) {

			// Update Last Milestone Index
			LastMilestoneIndex = i;

			// Broadcast Milestone Reached Event Dispatcher - Updates UI
			OnScoreMilestoneReached.Broadcast(i);

			// Was It The Final Milestone? (5 Stars)
			if (i == ScoreMilestones.Num() - 1) {
				UE_LOG(LogTemp, Log, TEXT("GameState: Final milestone reached! Score: %f"), CurrentScore);

				// Stop Timer
				StopLevelTimer();

				// Change A Bool Var To Call OnRep_LevelComplete For Clients To Get The UI Aswell
				LevelComplete = true;

				// Broadcast Level Complete Dispatcher
				OnLevelComplete.Broadcast();

				// Pause The Game
				//UGameplayStatics::SetGamePaused(GetWorld(), true);
			}
		}
	}
}

void AMyGameState::OnRep_MilestoneIndex()
{
	OnScoreMilestoneReached.Broadcast(LastMilestoneIndex);
}

void AMyGameState::LoadNextLevel() const
{
	if (NextLevel.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState: No next level set!"));
		return;
	}

	// Get Level Path From Soft Object Ptr
	FString LevelPath = NextLevel.GetLongPackageName();

	// Travel To The Next Level
	GetWorld()->ServerTravel(LevelPath + "?listen");
}

// Is There A Next Level Assigned - Used For Disabling The Next Level Button On Game Over UI
bool AMyGameState::HasNextLevel()
{

	if (!NextLevel.IsNull())
	{
		return true;
	}

	return false;
}

void AMyGameState::ReplayLevel() const
{
	// Get Current Level Name
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);

	// Travel Back To Current Level To Restart It
	GetWorld()->ServerTravel(CurrentLevelName + "?listen");
}

void AMyGameState::GoToLobby() const
{
	// Get Level Path From Soft Object Ptr
	FString LevelPath = LobbyLevel.GetLongPackageName();

	// Travel To The Next Level
	GetWorld()->ServerTravel(LevelPath + "?listen");
}

void AMyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMyGameState, TimeRemaining);
	DOREPLIFETIME(AMyGameState, CurrentScore);
	DOREPLIFETIME(AMyGameState, LastMilestoneIndex);
	DOREPLIFETIME(AMyGameState, LevelComplete);
	DOREPLIFETIME(AMyGameState, NextLevel)
}

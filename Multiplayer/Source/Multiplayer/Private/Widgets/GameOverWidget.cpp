// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameOverWidget.h"
#include <MyGameState.h>
#include <Kismet/GameplayStatics.h>


void UGameOverWidget::NativeConstruct()
{
	
	// Get Owning Player Controller & Check If Its The Server Or A Client
	APlayerController* PC = GetOwningPlayer();

	// If Server
	if (PC && PC->HasAuthority()) {

		// Bind Next Level Button Pressed Event
		if (NextLevelButton) { NextLevelButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnNextLevelButtonPressed); }

		// Bind Replay Level Button Pressed Event
		if (ReplayLevelButton) { ReplayLevelButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnReplayLevelButtonPressed); }
	}

}

void UGameOverWidget::SetupUI(float TotalScore, int32 Stars)
{

	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameOverWidget Total Score %.2f")); }

	// Convert Score To String
	FString TotalScoreString = FString::Printf(TEXT("Total Score: $%.2f"), TotalScore);

	// Set Toal Score Text To The String That Was Just Made
	TotalScoreText->SetText(FText::FromString(TotalScoreString));

	// Call Blueprint Event To Update Star UI
	UpdateStarUI(Stars);
}

void UGameOverWidget::OnNextLevelButtonPressed()
{
	// Get Game State
	AMyGameState* GS = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	if (GS){

		// Call Function To Change Level
		GS->LoadNextLevel();
	}
}

void UGameOverWidget::OnReplayLevelButtonPressed()
{
	// Get Game State
	AMyGameState* GS = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	if (GS) {

		// Call Function To Change Level
		GS->ReplayLevel();
	}
}


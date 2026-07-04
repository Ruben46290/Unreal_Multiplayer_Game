// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameOverWidget.h"
#include <MyGameState.h>
#include <Kismet/GameplayStatics.h>
#include <MyGameInstance.h>
#include <MyPlayerController.h>


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

		// Bind Lobby Button Pressed Event
		if (LobbyButton) { LobbyButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnLobbyButtonPressed); }
	}
}

void UGameOverWidget::SetupUI(float TotalScore, int32 Stars, float TimeRemaining)
{

	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameOverWidget Total Score %.2f")); }

	// Convert Score To String
	FString TotalScoreString = FString::Printf(TEXT("Total Score: $%.2f"), TotalScore);

	// Set Toal Score Text To The String That Was Just Made
	TotalScoreText->SetText(FText::FromString(TotalScoreString));

	// Call Blueprint Event To Update UI
	UpdateUI(Stars, TimeRemaining);

	// Get Game Instance
	if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
	{
		// Is The Game Singleplayer?
		if (GI->bIsSingleplayer)
		{
			// Disable The Lobby Button
			LobbyButton->bIsEnabled = false;
		}

		// Game Is Multiplayer 
		else {

			// Get Owning Player Controller & Check If Its The Server Or A Client
			APlayerController* PC = GetOwningPlayer();

			// If Player 2
			if (PC && !PC->HasAuthority()) {

				// Disable All Buttons
				NextLevelButton->bIsEnabled = false;
				ReplayLevelButton->bIsEnabled = false;
				LobbyButton->bIsEnabled = false;
			}
		}
	}

	// Get Game State
	AMyGameState* GS = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));
	if (GS) {

		// If There Isn't A Next Level Assigned - Disable The Next Level Button
		if (!GS->HasNextLevel()) {
			NextLevelButton->bIsEnabled = false;
		}
	}
}

void UGameOverWidget::OnNextLevelButtonPressed()
{
	// Get Game State
	AMyGameState* GS = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	if (GS){

		// Call Function To Change Level
		GS->LoadNextLevel();
	}
	ShowLoadingScreens();
}

void UGameOverWidget::OnReplayLevelButtonPressed()
{
	// Get Game State
	AMyGameState* GS = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	if (GS) {

		// Call Function To Change Level
		GS->ReplayLevel();
	}
	ShowLoadingScreens();
}

void UGameOverWidget::OnLobbyButtonPressed()
{
	// Get Game State
	AMyGameState* GS = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	if (GS) {

		// Call Function To Change Level
		GS->GoToLobby();
	}
	ShowLoadingScreens();
}

// Show Loading Screen On All PLayers (Only Called By Server)
void UGameOverWidget::ShowLoadingScreens()
{
	// Loop Through All PLayer Controllers
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// Cast To MyPlayerController
		AMyPlayerController* PC = Cast<AMyPlayerController>(It->Get());
		if (PC)
		{
			// Show Loading Screen
			PC->Client_ShowLoadingScreen();
		}
	}
}


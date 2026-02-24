// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyHUD.h"
#include "Lobby_MainMenu/LobbyPlayerController.h"


void ULobbyHUD::NativeConstruct()
{
	// Bind Buttons
	if (ReadyButton) {
		ReadyButton->OnClicked.AddDynamic(this, &ULobbyHUD::OnReadyButtonPressed);
	}

	// Get Owning Player Controller & Check If Its The Server Or A Client
	APlayerController* PC = GetOwningPlayer();

	// If Server
	if (PC && PC->HasAuthority()) {

		// Bind Level Selection Buttons For The Server Only
		if (LeftArrowButton)
		{
			LeftArrowButton->OnClicked.AddDynamic(this, &ULobbyHUD::OnLeftArrowClicked);
		}

		if (RightArrowButton)
		{
			RightArrowButton->OnClicked.AddDynamic(this, &ULobbyHUD::OnRightArrowClicked);
		}
	}

	else 	{

		// Hide Level Selection Buttons For Clients
		if (LeftArrowButton)
		{
			LeftArrowButton->SetIsEnabled(false);
			LeftArrowButton->SetVisibility(ESlateVisibility::Hidden);
		}
		if (RightArrowButton)
		{
			RightArrowButton->SetIsEnabled(false);
			RightArrowButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}



	// Set Inital Level Display - Shows Level 1 For Now
	UpdateLevelDisplay(1);

}

void ULobbyHUD::OnReadyButtonPressed()
{
	// Get Owning Character & Cast To Lobby Player Controller
	ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(GetOwningPlayer());

	// If Player Controller Valid
	if (LobbyPC)
	{
		// Toggle Ready Status
		LobbyPC->ToggleReady();

		// Player Controller Function Calls The Update UI Blueprint Event
	}
}

void ULobbyHUD::OnLeftArrowClicked()
{
	// Get Player Controller
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->Server_ChangeLevel(-1); // Previous Level
	}
}

void ULobbyHUD::OnRightArrowClicked()
{
	// Get Player Controller
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->Server_ChangeLevel(1); // Next Level
	}
}


void ULobbyHUD::UpdateLevelDisplay(int32 Level)
{
	
	CurrentLevel = Level;


	if (LevelText)
	{
		LevelText->SetText(FText::FromString(FString::Printf(TEXT("Level %d"), Level)));
	}

	// Optional: Grey out arrows at min/max
	if (LeftArrowButton)
	{
		LeftArrowButton->SetIsEnabled(Level > 1);
	}

	if (RightArrowButton)
	{
		RightArrowButton->SetIsEnabled(Level < 10);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyHUD.h"
#include "Lobby_MainMenu/LobbyPlayerController.h"
#include "Lobby_MainMenu/LobbyCharacter.h"

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


	// Bind Other Buttons
	if (BackButton) {
		BackButton->OnClicked.AddDynamic(this, &ULobbyHUD::OnBackButtonClicked);
	}
	if (SkinLeftButton) {
		SkinLeftButton->OnClicked.AddDynamic(this, &ULobbyHUD::OnSkinLeftButtonClicked);
	}
	if (SkinRightButton) {
		SkinRightButton->OnClicked.AddDynamic(this, &ULobbyHUD::OnSkinRightButtonClicked);
	}


	// Set Inital Level Display - Shows Level 1 For Now
	UpdateLevelDisplay(1);

}

void ULobbyHUD::OnBackButtonClicked()
{
	// Get Player Controller
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->LeaveSession();
	}
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


// * * * * * Level Selection * * * * *

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




// * * * * * Skin Selection * * * * *

void ULobbyHUD::OnSkinLeftButtonClicked()
{
	CurrentSkinIndex = (CurrentSkinIndex - 1 + MaxSkins) % MaxSkins;
	UpdateSkinText();
	ApplySkinToCharacter();
}

void ULobbyHUD::OnSkinRightButtonClicked()
{
	CurrentSkinIndex = (CurrentSkinIndex + 1) % MaxSkins;
	UpdateSkinText();
	ApplySkinToCharacter();
}



void ULobbyHUD::ApplySkinToCharacter()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	ALobbyCharacter* LobbyChar = Cast<ALobbyCharacter>(PC->GetPawn());
	if (!LobbyChar) return;

	// Instantly Aoply The Skin To The Character
	LobbyChar->ApplySkin(CurrentSkinIndex);

	// Tell The Server To Update The Skin For All Clients - Small Delay But Ensures All Clients See The Same Skin
	LobbyChar->LocalSelectSkin(CurrentSkinIndex);
}

void ULobbyHUD::UpdateSkinText()
{
	
	FString DisplayText = SkinNames[CurrentSkinIndex];

	SkinText->SetText(FText::FromString(DisplayText));
}

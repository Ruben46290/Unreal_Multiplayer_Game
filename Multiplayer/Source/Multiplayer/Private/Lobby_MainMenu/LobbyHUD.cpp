// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyHUD.h"
#include "Lobby_MainMenu/LobbyPlayerController.h"


void ULobbyHUD::NativeConstruct()
{
	if (ReadyButton) {
		ReadyButton->OnClicked.AddDynamic(this, &ULobbyHUD::OnReadyButtonPressed);
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












//HostLANButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostLANButtonClicked);
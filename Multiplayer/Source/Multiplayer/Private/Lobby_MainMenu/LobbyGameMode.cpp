// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "../MultiplayerCharacter.h"


ALobbyGameMode::ALobbyGameMode()
{
	// Set Player Controller To Lobby Controller
	PlayerControllerClass = ALobbyPlayerController::StaticClass();

	// Keep Using The Normal Player - Could Replace With A Custom Character Class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Lobby_MainMenu/Blueprints/BP_LobbyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}


	// Reset Player Spawn Index
	PlayerSpawnIndex = 0;
}








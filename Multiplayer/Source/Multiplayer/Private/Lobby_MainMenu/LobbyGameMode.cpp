// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyGameMode.h"
#include "LobbyPlayerController.h"
#include "LobbyCharacter.h"


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

// * * * * * * * * * * Ready * * * * * * * * * *

// When A Player Changes There Ready Status
void ALobbyGameMode::OnPlayerReadyChanged()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, TEXT("Player ready status changed, checking..."));
	}

	// Check If All Players Are Ready
	if (AreAllPlayersReady())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("All players ready! Starting game in 3 seconds..."));
		}

		// Start game after a short delay
		FTimerHandle StartGameTimer;
		GetWorldTimerManager().SetTimer(StartGameTimer, this, &ALobbyGameMode::StartGame, 3.0f, false);
	}
}

bool ALobbyGameMode::AreAllPlayersReady()
{
	// Get all player controllers
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			// Get their pawn
			ALobbyCharacter* LobbyChar = Cast<ALobbyCharacter>(PC->GetPawn());
			if (LobbyChar)
			{
				// If any player is not ready, return false
				if (!LobbyChar->GetIsReady())
				{
					return false;
				}
			}
		}
	}

	// All players are ready
	return true;
}

void ALobbyGameMode::StartGame()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Starting game!"));
	}

	// Travel to the game map
	GetWorld()->ServerTravel(GameMapPath + "?listen");
}





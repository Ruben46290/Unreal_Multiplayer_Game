// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "MultiplayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyGameState.h"
#include "MyPlayerController.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Set Gane State Class
	GameStateClass = AMyGameState::StaticClass();

	// Set Player Controler
	PlayerControllerClass = AMyPlayerController::StaticClass();

}

void AMultiplayerGameMode::BeginPlay()
{
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("GameMode BeginPlay()")); }

	// Get Game State
	AMyGameState* GS = GetGameState<AMyGameState>();

	// If Game State is Valid
	if (GS) {

		// Start Level Timer
		GS -> StartLevelTimer();
	}


}


void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Add Player To Array
	LoadedPlayers.Add(NewPlayer);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		FString::Printf(TEXT("Player loaded: %d/%d"), LoadedPlayers.Num(), ExpectedPlayerCount));

	// Check If All The Players Have Been Loaded In
	CheckAllPlayersLoaded();
}


void AMultiplayerGameMode::OnPlayerLoaded()
{
}

void AMultiplayerGameMode::CheckAllPlayersLoaded()
{
	// If All Players Are Loaded In & The Countdown Isn't Active
	if (LoadedPlayers.Num() >= ExpectedPlayerCount && !bCountdownStarted)
	{
		// Set Countdown As Active
		bCountdownStarted = true;

		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("All players loaded! Starting countdown..."));

		// For Each Loaded Player
		for (APlayerController* PC : LoadedPlayers)
		{
			// Cast To MyPlayerController
			AMyPlayerController* GamePC = Cast<AMyPlayerController>(PC);
			if (GamePC)
			{
				// Start Countdown
				GamePC->Client_StartGameCountdown();
			}
		}
	}
}


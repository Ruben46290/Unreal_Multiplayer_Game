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

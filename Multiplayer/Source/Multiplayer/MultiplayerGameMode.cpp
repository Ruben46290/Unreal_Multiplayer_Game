// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerGameMode.h"
#include "MultiplayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "MyGameState.h"
#include "MyPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include "Ordering/OrderManager.h"
#include "MyPlayerState.h"
#include "MyGameInstance.h"

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

	// Set Player State Class
	PlayerStateClass = AMyPlayerState::StaticClass();

	// Enable Seamless Travel - Carries Over Player State Instead Of Resetting It
	bUseSeamlessTravel = false;
}

void AMultiplayerGameMode::BeginPlay()
{
	// Load Level Settings From LevelSettings Actor
	LoadLevelSettings();
}


void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Add Player To Array
	LoadedPlayers.Add(NewPlayer);

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
	//	FString::Printf(TEXT("Player loaded: %d/%d"), LoadedPlayers.Num(), ExpectedPlayerCount));

}

void AMultiplayerGameMode::LoadLevelSettings()
{
	// Find the LevelSettings actor in the level
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelSettings::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		LevelSettings = Cast<ALevelSettings>(FoundActors[0]);

		if (LevelSettings)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
				TEXT("LevelSettings actor found"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			TEXT("No LevelSettings actor found in level! Using defaults."));
	}
}




void AMultiplayerGameMode::CheckAllPlayersLoaded()
{
	// Get Game Instance
	if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
	{
		// Is The Game Singleplayer?
		if (GI->bIsSingleplayer)
		{
			// Yes - Skip Multiplayer Logic
			StartSingleplayer();
			return;
		}
	}

	// If All Players Are Loaded In & The Countdown Isn't Active
	if (LoadedPlayers.Num() >= ExpectedPlayerCount && !bCountdownStarted)
	{

		// Apply skins for all players now that everyone is loaded
		for (APlayerController* PC : LoadedPlayers)
		{
			if (AMultiplayerCharacter* GameChar = Cast<AMultiplayerCharacter>(PC->GetPawn()))
			{
				if (APlayerState* PS = PC->GetPlayerState<APlayerState>())
				{
					if (UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>())
					{
						int32 SkinIndex = GI->GetPlayerSkin(PS->GetPlayerName());
						UE_LOG(LogTemp, Warning, TEXT("CheckAllPlayersLoaded - applying skin %d to %s"), SkinIndex, *PS->GetPlayerName());
						GameChar->ApplySkin(SkinIndex);
					}
				}
			}

			// Set Countdown As Active
			bCountdownStarted = true;

			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("All players loaded! Starting countdown..."));

			// For Each Loaded Player
			for (APlayerController* PPC : LoadedPlayers)
			{
				// Cast To MyPlayerController
				AMyPlayerController* GamePPC = Cast<AMyPlayerController>(PPC);
				if (GamePPC)
				{
					// Start Countdown
					GamePPC->Client_StartGameCountdown();
				}
			}
		}
	}
}


void AMultiplayerGameMode::StartGameplay()
{
	// Get Game Instance
	if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
	{
		// Is The Game Singleplayer?
		if (GI->bIsSingleplayer)
		{
			// Yes - Skip Multiplayer Logic
			StartSingleplayerGameplay();
			return;
		}
	}

	ReadyPlayerCount++;

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
		FString::Printf(TEXT("Players ready: %d/%d"), ReadyPlayerCount, ExpectedPlayerCount));

	// Check if all players have finished countdown
	if (ReadyPlayerCount >= ExpectedPlayerCount)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("All players ready! Starting game timer!"));

		// NOW start the timer
		AMyGameState* GS = GetGameState<AMyGameState>();

		if (!GS) {
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("MultiplayerGameMoode - StartGameplay() - Cast To GameState Failed"));
			return;
		}

		// If Level Settings Are Valid
		if (LevelSettings)
		{
			GS->StartLevelTimer(LevelSettings->GameLength);
		}

		// No Settings Loaded
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MultiplayerGameMoode - StartGameplay() - No Settings Loaded!"));
			GS->StartLevelTimer(120.f); // Fall Back To 2 Minute Timer
		}

		// Only Run On Server
		if (HasAuthority()) {
			// Enable OrderManager Tick
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrderManager::StaticClass(), FoundActors);

			if (FoundActors.Num() > 0)
			{
				AOrderManager* OrderMgr = Cast<AOrderManager>(FoundActors[0]);
				if (OrderMgr)
				{
					OrderMgr->SetActorTickEnabled(true);
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("MultiplayerGameMoode - StartGameplay() - ! No Order Manager Found !"));
			}
		}
	}
}

void AMultiplayerGameMode::StartSingleplayer()
{
	// Check Player Controller Is Valid
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetWorld()->GetFirstPlayerController());
	if (!PC) return;

	// Apply Skin 
	if (AMultiplayerCharacter* GameChar = Cast<AMultiplayerCharacter>(PC->GetPawn()))
	{
		if (UMyGameInstance* GI = GetGameInstance<UMyGameInstance>())
		{
			int32 SkinIndex = GI->GetPlayerSkin("Local");
			UE_LOG(LogTemp, Warning, TEXT("Singleplayer skin index: %d"), SkinIndex);
			GameChar->ApplySkin(SkinIndex);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StartSingleplayer - Character cast failed"));
	}

	// Start Countdown
	PC->Client_StartGameCountdown();
}

void AMultiplayerGameMode::StartSingleplayerGameplay()
{
	// Get GameState
	AMyGameState* GS = GetGameState<AMyGameState>();
	if (!GS) { return; }

	// If Level Settings Are Valid
	if (LevelSettings)
	{
		// Load Level Time + Singleplayer % Boost
		GS->StartLevelTimer(LevelSettings->GameLength + (LevelSettings->GameLength * LevelSettings->TimePatienceBoost));
	}

	// No Settings Loaded
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MultiplayerGameMoode - StartGameplay() - No Settings Loaded!"));
		GS->StartLevelTimer(120.f); // Fall Back To 2 Minute Timer
	}

	// Enable OrderManager
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrderManager::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		if (AOrderManager* OrderMgr = Cast<AOrderManager>(FoundActors[0]))
		{
			OrderMgr->SetActorTickEnabled(true);
		}
	}
}




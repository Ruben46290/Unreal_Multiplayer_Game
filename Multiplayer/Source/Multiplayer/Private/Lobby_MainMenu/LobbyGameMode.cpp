// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyGameMode.h"
#include "Lobby_MainMenu/LobbyPlayerController.h"
#include "Lobby_MainMenu/LobbyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MyPlayerState.h"

ALobbyGameMode::ALobbyGameMode()
{
	// Set Player Controller To Lobby Controller
	PlayerControllerClass = ALobbyPlayerController::StaticClass();

	// Set Player State Class
	PlayerStateClass = AMyPlayerState::StaticClass();
	
	// Enable Seamless Travel - Carries Over Player State Instead Of Resetting It
	bUseSeamlessTravel = false;

	// Keep Using The Normal Player - Could Replace With A Custom Character Class
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Lobby_MainMenu/Blueprints/BP_LobbyCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// Reset Player Spawn Index
	PlayerSpawnIndex = 0;

	// Set Default Level Paths
	LevelPaths = {
	"/Game/Maps/TestMap",
	"/Game/Maps/TestMap1",
	};
}


// Called When A Player Loads In
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	FTimerHandle UpdateTimer;
	GetWorldTimerManager().SetTimer(UpdateTimer, this, &ALobbyGameMode::UpdateReadyButtonAvailability, 0.5f, false);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("GameMode PostLogin()"));
}



void ALobbyGameMode::UpdateReadyButtonAvailability()
{
	// Get Amount Of Players
	int32 PlayerCount = GetNumPlayers();

	// Set Can Ready Var To True If Theres 2 Players
	bool bCanReady = (PlayerCount == 2);
	
	// Broadcast Ready Button Status To All Clients & Server
	Multicast_UpdateReadyButtonState(bCanReady);

	//if (bCanReady) {
	//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("GameMode There Is 2 Players"));
	//}
	//else {
	//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("GameMode There Is 1 Player"));
	//}
}

void ALobbyGameMode::Multicast_UpdateReadyButtonState_Implementation(bool bCanReady)
{
	// Loop Through All Players
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		// Cast To Lobby Player Controller
		ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(It->Get());

		if (LobbyPC)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Update Ready Buttons"));
			LobbyPC->UpdateReadyButtonAvailability(bCanReady);
		}
	}
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
		GetWorldTimerManager().SetTimer(StartGameTimer, this, &ALobbyGameMode::StartGame, 1.0f, false);
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

	FString SelectedLevelPath = GameMapPath;

	if (LevelPaths.IsValidIndex(SelectedLevel - 1))
	{
		SelectedLevelPath = LevelPaths[SelectedLevel - 1];
	}

	// Travel to the game map
	GetWorld()->ServerTravel(SelectedLevelPath + "?listen");
}


// * * * * * * * * * * Level Selection * * * * * * * * * *

void ALobbyGameMode::ChangeSelectedLevel(int32 NewLevel)
{
	SelectedLevel = FMath::Clamp(NewLevel, 1, LevelPaths.Num());

	UE_LOG(LogTemp, Warning, TEXT("[SERVER] ChangeSelectedLevel: New Level = %d"), SelectedLevel);

	// Update ALL player controllers (server and clients)
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(It->Get());
		if (LobbyPC)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SERVER] Calling Client_UpdateLevelDisplay on controller"));
			LobbyPC->Client_UpdateLevelDisplay(SelectedLevel);
		}
	}
}

void ALobbyGameMode::Multicast_UpdateLevelDisplay_Implementation(int32 Level)
{
	GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Cyan,
		TEXT("Multicast_UpdateLevelDisplay Called"));

	// Update all local controllers
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ALobbyPlayerController* LobbyPC = Cast<ALobbyPlayerController>(It->Get());
		if (LobbyPC && LobbyPC->IsLocalController())
		{
			LobbyPC->UpdateLevelDisplay(Level);
		}
	}
}


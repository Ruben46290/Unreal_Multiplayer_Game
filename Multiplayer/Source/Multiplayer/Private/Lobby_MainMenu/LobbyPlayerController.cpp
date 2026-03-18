// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyPlayerController.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Blueprint/UserWidget.h"
#include <Lobby_MainMenu/LobbyCharacter.h>
#include <Lobby_MainMenu/LobbyGameMode.h>
#include <MyGameInstance.h>




void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Connect To Camera On The Level & Teleport Player Mesh To Proper Spot
	SetupLobbyCamera();

	// Make The Lobby UI
	CreateLobbyUI();

	// Connect To Camera On The Level & Teleport Player Mesh To Proper Spot
	FTimerHandle UpdateTimer;
	GetWorldTimerManager().SetTimer(UpdateTimer, this, &ALobbyPlayerController::SetupLobbyCamera, 1.0f, false);
}


void ALobbyPlayerController::SetupLobbyCamera()
{
	// Find The Camera Actor With Tag 'LobbyCamera'
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACameraActor::StaticClass(), FName("LobbyCamera"), FoundCameras);

	if (FoundCameras.Num() > 0)
	{
		LobbyCameraActor = FoundCameras[0];
		SetViewTargetWithBlend(LobbyCameraActor, 0.0f);

		APawn* MyPawn = GetPawn();
		if (MyPawn)
		{
			// Teleport The Player To The Right Spawnpoint
			TeleportPlayerToSpawn();

			// Disable Input
			MyPawn->DisableInput(this);
		}

	}
}

void ALobbyPlayerController::TeleportPlayerToSpawn()
{
	// Get Owning Pawn
	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;

	// Get Player State
	APlayerState* PS = GetPlayerState<APlayerState>();
	if (!PS) return;

	// Get the game state to count players
	AGameStateBase* GameState = GetWorld()->GetGameState();
	if (!GameState) return;

	// Find this player's index in the PlayerArray
	int32 PlayerIndex = GameState->PlayerArray.Find(PS);

	FName SpawnTag = (PlayerIndex == 0) ? FName("Player1") : FName("Player2");

	// Find Spawn Point With Matching Tag
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), SpawnTag, SpawnPoints);

	if (SpawnPoints.Num() > 0)
	{
		FVector NewSpawnLocation = SpawnPoints[0]->GetActorLocation();
		FRotator NewSpawnRotation = SpawnPoints[0]->GetActorRotation();
		MyPawn->SetActorLocationAndRotation(NewSpawnLocation, NewSpawnRotation);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Teleported Player %d"), PlayerIndex));
	}
}

void ALobbyPlayerController::CreateLobbyUI()
{
	// Only Run On Local Machine
	if (!IsLocalController()) { return; }

	// If HUD Blueprint Class Was Set
	if (LobbyWidgetClass)
	{
		// Create The Widget
		LobbyWidget = CreateWidget<ULobbyHUD>(this, LobbyWidgetClass);

		// If Widget Is Valid
		if (LobbyWidget)
		{
			// Add Widget To Viewport
			LobbyWidget->AddToViewport();

			// Set Input Mode To UI Only & Show Mouse Cursor
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(LobbyWidget->TakeWidget());
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
	}
}

// Called By The GameMode To Disable / Enable The Ready Button
void ALobbyPlayerController::UpdateReadyButtonAvailability(bool bCanReady)
{
	// Pass Along To The Widget
	if (LobbyWidget) {
		LobbyWidget->SetReadyButtonEnabled(bCanReady);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("LobbyPlayerController - UpdateReadyButtonAvailability - Lobby Widget Invalid"));
	}
}

void ALobbyPlayerController::ToggleReady()
{
	// Get Owning Pawn & Cast To ALobbyCharacter
	ALobbyCharacter* LobbyChar = Cast<ALobbyCharacter>(GetPawn());

	// If Character Refrence Valid
	if (LobbyChar)
	{
		// Flip bIsReady bool (true -> false || false -> true)
		bool bNewReady = !LobbyChar->GetIsReady();

		// Set The Server Status To New Ready Status
		LobbyChar->Server_SetReady(bNewReady);

		// Update Ready Button UI
		LobbyWidget->UpdateReadyButtonStatus(bNewReady);
	}
}

void ALobbyPlayerController::UpdateLevelDisplay(int32 Level)
{
	// Called From LobbyGameMode When Host Clicks Arrow Buttons To Change Level
	if (LobbyWidget)
	{
		LobbyWidget->UpdateLevelDisplay(Level);
	}
}

void ALobbyPlayerController::Client_UpdateLevelDisplay_Implementation(int32 Level)
{
	//UE_LOG(LogTemp, Warning, TEXT("[CLIENT RPC] Client_UpdateLevelDisplay called with Level %d"), Level);
	UpdateLevelDisplay(Level);
}


void ALobbyPlayerController::Server_ChangeLevel_Implementation(int32 Direction)
{
	// Get GameMode & Cast To ALobbyGameMode
	ALobbyGameMode* GM = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		// Calculate New Level Based On Direction (-1 = Left Arrow, +1 = Right Arrow)
		int32 NewLevel = GM->GetSelectedLevel() + Direction;
		GM->ChangeSelectedLevel(NewLevel);
	}
}


void ALobbyPlayerController::Client_ShowLoadingScreen_Implementation()
{
	if (LoadingScreenClass)
	{
		// Remove lobby UI
		if (LobbyWidget)
		{
			LobbyWidget->RemoveFromParent();
		}

		// Create loading screen
		LoadingScreen = CreateWidget<ULoadingScreenWidget>(this, LoadingScreenClass);
		if (LoadingScreen)
		{
			LoadingScreen->AddToViewport(999); // High Z-order to be on top

			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Loading screen shown"));
		}
	}
}



void ALobbyPlayerController::LeaveSession()
{
	// Called From LobbyHUD Back Button

	// Get Game Instance
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem) {

		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->DestroySession(NAME_GameSession);
		}
	}

	UGameplayStatics::OpenLevel(this, FName("MainMenuMap"));
}
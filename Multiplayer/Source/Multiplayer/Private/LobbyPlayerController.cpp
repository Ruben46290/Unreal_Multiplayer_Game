// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameStateBase.h"


void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupLobbyCamera();
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

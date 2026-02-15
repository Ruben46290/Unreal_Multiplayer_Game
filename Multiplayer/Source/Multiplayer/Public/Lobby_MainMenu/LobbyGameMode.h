// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Controller.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ALobbyGameMode();


	// * * * * * * * * * * Ready * * * * * * * * * *
public:
	// Called when a player's ready status changes
	UFUNCTION()
	void OnPlayerReadyChanged();

	// Start the game (travel to game map)
	UFUNCTION()
	void StartGame();

protected:
	// Check if all players are ready
	bool AreAllPlayersReady();

	// The map to travel to when game starts
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FString GameMapPath = "/Game/Maps/TestMap";


private:

	// What Spawn Index Should The Player Spawn At
	int32 PlayerSpawnIndex = 0;
};
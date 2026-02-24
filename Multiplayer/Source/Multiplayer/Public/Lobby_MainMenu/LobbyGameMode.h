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

	// * * * Ready Button * * *
public:

	// Override Login Event
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Multicast to tell all clients to update their ready button state
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateReadyButtonState(bool bCanReady);

	// Check player count and update ready buttons
	void UpdateReadyButtonAvailability();


private:

	// What Spawn Index Should The Player Spawn At
	int32 PlayerSpawnIndex = 0;


	// * * * * * * * * * * Level Selection * * * * * * * * * *
protected:

	// Currently selected level
	UPROPERTY(BlueprintReadOnly, Category = "Lobby")
	int32 SelectedLevel = 1;

	// Map paths for each level
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	TArray<FString> LevelPaths;

public:
	// Change the selected level (host only)
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void ChangeSelectedLevel(int32 NewLevel);

	// Get current selected level
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	int32 GetSelectedLevel() const { return SelectedLevel; }

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateLevelDisplay(int32 Level);

};
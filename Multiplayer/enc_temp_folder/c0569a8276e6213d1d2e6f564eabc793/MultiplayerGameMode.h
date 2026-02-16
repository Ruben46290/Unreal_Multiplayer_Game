// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiplayerGameMode.generated.h"

UCLASS(minimalapi)
class AMultiplayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMultiplayerGameMode();

	void BeginPlay();

	// * * * * * * * * * * Loading Into Level * * * * * * * * * *
public:

	// Called When A Player Loads Into The Level
	void OnPlayerLoaded();

	// Check If All The Players Are Loaded In
	void CheckAllPlayersLoaded();

protected:
	// Override Login Event
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:

	
	TArray<APlayerController*> LoadedPlayers; // Array Of Loaded Players
	int32 ExpectedPlayerCount = 2; // How Many Players Are In The Game - Hardcoded To 2 For This Whole Game
	bool bCountdownStarted = false; // Is The Countdown Going
};




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Lobby_MainMenu/LobbyHUD.h"
#include "LoadingScreenWidget.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	AActor* LobbyCameraActor;

	// Connect To Camera Placed In The Lobby Level
	void SetupLobbyCamera();

	// Teleport To The Right Spawn Pos
	void TeleportPlayerToSpawn();

	// * * * * * * * * * * UI * * * * * * * * *

protected:

	// Blueprint Class For Lobby UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULobbyHUD> LobbyWidgetClass;

	// Refrence
	UPROPERTY()
	ULobbyHUD* LobbyWidget;

	void CreateLobbyUI();

public:

	// Called By The GameMode To Disable / Enable The Ready Button
	void UpdateReadyButtonAvailability(bool bCanReady);


	// Called by the UI button
	UFUNCTION(BlueprintCallable)
	void ToggleReady();

	UFUNCTION()
	void LeaveSession();

	// * * * * * * * * * * Loading Screen * * * * * * * * * * 
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoadingScreenWidget> LoadingScreenClass;

	UPROPERTY()
	ULoadingScreenWidget* LoadingScreen;

public:
	UFUNCTION(Client, Reliable)
	void Client_ShowLoadingScreen();


	// * * * * * * * * * * Level Display * * * * * * * * * * 
public:

	UFUNCTION(Client, Reliable)
	void Client_UpdateLevelDisplay(int32 Level);

	// Update the level display
	void UpdateLevelDisplay(int32 Level);

	// Called when host clicks arrow buttons
	UFUNCTION(Server, Reliable)
	void Server_ChangeLevel(int32 Direction); // +1 or -1
};

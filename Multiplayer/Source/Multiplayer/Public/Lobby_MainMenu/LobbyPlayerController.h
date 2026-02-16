// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Lobby_MainMenu/LobbyHUD.h"
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
	// Called by the UI button
	UFUNCTION(BlueprintCallable)
	void ToggleReady();


	// * * * * * * * * * * Loading Screen * * * * * * * * * * 
protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoadingScreenWidget> LoadingScreenClass;

	UPROPERTY()
	ULoadingScreenWidget* LoadingScreen;

public:
	UFUNCTION(Client, Reliable)
	void Client_ShowLoadingScreen();
};

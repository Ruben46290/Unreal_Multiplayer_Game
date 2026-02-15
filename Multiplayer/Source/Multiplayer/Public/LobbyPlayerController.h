// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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
};

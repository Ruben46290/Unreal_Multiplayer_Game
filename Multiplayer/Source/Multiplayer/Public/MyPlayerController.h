// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/GameOverWidget.h"
#include "Widgets/GameStartWidget.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	AMyPlayerController();


	void BeginPlay();

	//void ReceivedPlayer();

	// What Blueprint Class Is The Level Completed Widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameOverWidget> LevelCompleteWidgetClass;

	UPROPERTY()
	UGameOverWidget* LevelCompleteWidget;
	
	UFUNCTION()
	void BindToGameState();

	UFUNCTION()
	void ShowLevelCompleteScreen();

	// * * * * * * * * * * Game Start / Loading * * * * * * * * * *
protected:

	// Blueprint Class Of Game Start Widget - Needs To Be Set In Editor
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameStartWidget> GameStartWidgetClass;

	// Refrence To Game Start Widget
	UPROPERTY()
	UGameStartWidget* GameStartWidget;

	void ShowGameStartWidget();

public:
	UFUNCTION(Client, Reliable)
	void Client_StartGameCountdown();

	// Called when countdown finishes
	UFUNCTION(Server, Reliable)
	void Server_NotifyCountdownFinished();
};

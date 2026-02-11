// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameOverWidget.h"
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

	void ReceivedPlayer();

	// What Blueprint Class Is The Level Completed Widget
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UGameOverWidget> LevelCompleteWidgetClass;

	UPROPERTY()
	UGameOverWidget* LevelCompleteWidget;
	
	UFUNCTION()
	void BindToGameState();

	UFUNCTION()
	void ShowLevelCompleteScreen();
};

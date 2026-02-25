// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "LobbyHUD.generated.h"

/**
 * 
 */
UCLASS()
class ULobbyHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	// * * * * * Components * * * * *
public:
	// Public To Access In Blueprints
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ReadyButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LeftArrowButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RightArrowButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LevelText;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	// * * * * * Functions * * * * *
protected:

	UFUNCTION()
	void OnReadyButtonPressed();

	UFUNCTION()
	void OnLeftArrowClicked();

	UFUNCTION()
	void OnRightArrowClicked();

	UFUNCTION()
	void OnBackButtonClicked();

public:

	// Enable / Disable The Ready Button - Called From Gamemode -> PlayerController -> This Widget
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void SetReadyButtonEnabled(bool bEnabled);

	// Set Ready Button To Ready / Not Ready - Called From PlayerController
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void UpdateReadyButtonStatus(bool bIsReady);

	// Update Level Displayed On The UI - Called From PlayerController
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void UpdateLevelDisplay(int32 Level);

private:
	int32 CurrentLevel = 1;
};

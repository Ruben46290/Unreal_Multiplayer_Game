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


public:
	// Public To Access In Blueprints
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ReadyButton;

protected:

	UFUNCTION()
	void OnReadyButtonPressed();

public:

	// Enable / Disable The Ready Button - Called From Gamemode -> PlayerController -> This Widget
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void SetReadyButtonEnabled(bool bEnabled);

	// Set Ready Button To Ready / Not Ready - Called From PlayerController
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void UpdateReadyButtonStatus(bool bIsReady);
};

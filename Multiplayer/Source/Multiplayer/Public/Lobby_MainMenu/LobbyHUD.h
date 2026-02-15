// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
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

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateReadyButton(bool bIsReady);
};

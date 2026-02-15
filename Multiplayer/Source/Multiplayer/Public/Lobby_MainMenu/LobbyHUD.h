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
	
protected:

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* ReadyButton;

public:

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateReadyButton(bool bIsReady);
};

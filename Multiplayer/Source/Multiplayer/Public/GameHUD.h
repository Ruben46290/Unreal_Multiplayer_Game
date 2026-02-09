// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UGameHUD : public UUserWidget
{
	GENERATED_BODY()


protected:

	UPROPERTY(meta = (BindWidget));
	UTextBlock* TimerText;


	void NativeConstruct();

	UFUNCTION()
	void UpdateTimerText(float TimeRemaining);
};

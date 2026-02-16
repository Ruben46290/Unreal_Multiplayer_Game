// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameStartWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UGameStartWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// Blueprint Events For UI Updates / Events
	UFUNCTION(BlueprintImplementableEvent, Category = "GameStart")
	void PlayFadeInAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameStart")
	void UpdateStatusText(const FString& NewText);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameStart")
	void ShowCountdown(int32 CountdownNumber);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameStart")
	void HideWidget();

	//// Called By PlayerController When Loaded In
	//void SetPlayerReady();

	// Called To Start Countdown
	void StartCountdown();

protected:
	UFUNCTION()
	void OnCountdownTick();

	FTimerHandle CountdownTimer;
	int32 CountdownValue = 3;
	//bool bIsReady = false;
};

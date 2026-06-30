// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalScoreText;

	UPROPERTY(meta = (BindWidget))
	UButton* NextLevelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ReplayLevelButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LobbyButton;
public:

	UFUNCTION()
	void SetupUI(float TotalScore, int32 Stars, float TimeRemaining);

protected:

	UFUNCTION()
	void OnNextLevelButtonPressed();

	UFUNCTION()
	void OnReplayLevelButtonPressed();

	UFUNCTION()
	void OnLobbyButtonPressed();

	// Blueprint Event To Show Star UI
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateUI(int32 Stars, float TimeRemaining);
};

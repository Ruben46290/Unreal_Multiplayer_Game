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

	UPROPERTY(meta = (BindWidget));
	UTextBlock* ScoreText;


	void NativeConstruct();

	// Bound To Game State Timer Event Dispatcher
	UFUNCTION()
	void UpdateTimerText(float TimeRemaining);

	// Bound To Game State Score Event Dispatcher
	UFUNCTION()
	void UpdateScoreText(float NewScore);

	// Bound To Game State ScoreMilestoneHit Event Dispatcher
	UFUNCTION()
	void ScoreMilestoneReached(int32 CurrentStars);

	//Blueprint Event To Update Score Milestone UI
	UFUNCTION(BlueprintImplementableEvent, Category = "Score")
	void UpdateScoreMilestoneUI(int32 CurrentStars);
};

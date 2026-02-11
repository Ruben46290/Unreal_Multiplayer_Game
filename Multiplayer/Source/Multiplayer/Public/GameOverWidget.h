// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalScoreText;

public:

	UFUNCTION()
	void SetupUI(float TotalScore);
};

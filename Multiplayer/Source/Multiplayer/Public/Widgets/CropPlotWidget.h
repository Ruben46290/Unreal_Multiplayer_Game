// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "CropPlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UCropPlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget));
	UProgressBar* WaterProgressBar;

public:

	// Basically A Construct Function
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateCropUI(float WaterPercent);
};

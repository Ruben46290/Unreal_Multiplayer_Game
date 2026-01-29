// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
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

	UPROPERTY(meta = (BindWidget));
	UImage* CropTypeImage;

	UPROPERTY(meta = (BindWidget));
	UTextBlock* SeedCount;

public:

	bool bAutoRegrowEnabled = false;

	// Basically A Construct Function
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateSeedUI(UTexture2D* IconTexture);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateWaterUI(float WaterPercent);
};

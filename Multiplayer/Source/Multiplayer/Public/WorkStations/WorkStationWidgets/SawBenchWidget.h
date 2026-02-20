// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "SawBenchWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API USawBenchWidget : public UUserWidget
{
	GENERATED_BODY()
	
	// * * * * * Components * * * * *

	UPROPERTY(meta = (BindWidget))
	UProgressBar* SawProgressBar;

	UPROPERTY(meta = (BindWidget))
	UImage* LogImage;

	UPROPERTY(meta = (BindWidget))
	UImage* BucketImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LogCountText;

	// * * * * * Variables * * * * *

	UPROPERTY(EditAnywhere, Category = "Images")
	UTexture2D* LogTexture;

	UPROPERTY(EditAnywhere, Category = "Images")
	UTexture2D* NoLogTexture;

	UPROPERTY(EditAnywhere, Category = "Images")
	UTexture2D* BowlTexture;

	UPROPERTY(EditAnywhere, Category = "Images")
	UTexture2D* NoBowlTexture;
public:

	void UpdateLogCount(int32 LogCount);

	UFUNCTION(BlueprintCallable)
	void UpdateProgressBar(float Progress);

	void UpdateBucketUI(bool bHasBucket);
};

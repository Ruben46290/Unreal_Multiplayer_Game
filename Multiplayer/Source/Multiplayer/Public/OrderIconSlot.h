// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "OrderIconSlot.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UOrderIconSlot : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget));
	UImage* OrderIconImage;

	UPROPERTY(meta = (BindWidget));
	UTextBlock* OrderAmountText;

public:

	UFUNCTION(BlueprintCallable)
	void SetItemIcon(UTexture2D* IconTexture);

};

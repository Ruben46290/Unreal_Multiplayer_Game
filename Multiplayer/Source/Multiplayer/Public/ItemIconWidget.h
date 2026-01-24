// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ItemIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UItemIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	// Basically A Construct Function
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetIcon(UTexture2D* IconTexture);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "CustomerWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UCustomerWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PatienceBar;
	
public:

	UFUNCTION()
	void UpdateUI( float PatiencePercent);
};

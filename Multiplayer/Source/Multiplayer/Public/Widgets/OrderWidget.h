// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "OrderIconSlot.h"
#include "OrderWidget.generated.h"

struct FOrder;

UCLASS()
class MULTIPLAYER_API UOrderWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget));
	UWrapBox* OrderWrapBox;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* TimeProgressBar;

	UPROPERTY(meta = (BindWidget));
	UTextBlock* PointText;

protected:
	// Blueprint Class For OrderIconSlot
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UOrderIconSlot> OrderItemWidgetClass;

	// Data Table To Load Items Images - Needs To Be Set In Editor
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UDataTable* ItemDataTable;

public:

	void MakeOrder(TArray<FName> OrderItemss);

	void SetPointText(float Points);

	void ClearUI(bool CustomerRanOutOfTime);

	// Done In Blueprint To Allow For More Customization
	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePatienceBar(float TimePercentRemaining);

};

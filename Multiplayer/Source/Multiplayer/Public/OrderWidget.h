// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
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



};

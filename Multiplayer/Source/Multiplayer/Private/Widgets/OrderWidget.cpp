// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OrderWidget.h"
#include "Ordering/OrderManager.h"
#include <Data/ItemData.h>


void UOrderWidget::MakeOrder(TArray<FName> OrderItems)
{

    if (!OrderItemWidgetClass) {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("OrderWidget Trying To Spawn Icon But Icon Class Is Invalid!!!")); return; }
    }

    if (!ItemDataTable) {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("OrderWidget DataTable Not Set")); return; }
    }

	// Clear WrapBox Of Previous Images
    OrderWrapBox->ClearChildren();

    // For Each Required Item
    for (const FName& ItemName : OrderItems)
    {

        // Strip "EItemType::" prefix from the enum string
        FString ItemString = ItemName.ToString();
        FString CleanItemName;
        ItemString.Split(TEXT("::"), nullptr, &CleanItemName);
        FName CleanFName = FName(*CleanItemName);

        // Create A New Widget For Current Item
        UOrderIconSlot* ItemWidget = CreateWidget<UOrderIconSlot>(this, OrderItemWidgetClass);

        // If New Widget Is Valid
        if (ItemWidget) {

            // Load Row From Data Table
            FItemData* ItemData = ItemDataTable->FindRow<FItemData>(CleanFName, TEXT("Load From Order Widget"));

            if (!ItemData) {
                if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("ItemData NOT FOUND for: %s"), *ItemName.ToString())); return; }
            }

            // Apply Image
            ItemWidget->SetItemIcon(ItemData->OrderTexture);

            // And New Widget To The Wrap Box
            OrderWrapBox->AddChildToWrapBox(ItemWidget);
        }
    }

	// Show Time Progress Bar
	TimeProgressBar->SetVisibility(ESlateVisibility::Visible);
}

void UOrderWidget::SetPointText(float Points)
{
    // Convert Point Float To String
    FString PriceString = FString::Printf(TEXT("$%.2f"), Points);

    // Set Point Text To The String
    PointText->SetText(FText::FromString(PriceString));
}

void UOrderWidget::ClearUI(bool CustomerRanOutOfTime)
{
    // If The Customer Left Because They Have Been Waiting Too Long
    if (CustomerRanOutOfTime) {
        
        // Play Special Aniamtion
    }

    // Clear WrapBox Images
    OrderWrapBox->ClearChildren();

    // Set Point Text As Blank
    PointText->SetText(FText::FromString(""));

	// Hide Progress Bar
	TimeProgressBar->SetVisibility(ESlateVisibility::Hidden);
}

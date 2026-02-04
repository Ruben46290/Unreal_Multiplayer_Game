// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderWidget.h"
#include "OrderManager.h"
#include <ItemData.h>


void UOrderWidget::MakeOrder(FOrder NewOrder)
{

    if (!OrderItemWidgetClass) {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("OrderWidget Trying To Spawn Icon But Icon Class Is Invalid!!!")); return; }
    }

    if (!ItemDataTable) {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("OrderWidget DataTable Not Set")); return; }
    }

    OrderWrapBox->ClearChildren();

    // For Each Required Item
    for (const FName& ItemName : NewOrder.RequiredItems)
    {
        // Create A New Widget For Current Item
        UOrderIconSlot* ItemWidget = CreateWidget<UOrderIconSlot>(this, OrderItemWidgetClass);

        // If New Widget Is Valid
        if (ItemWidget) {

            // Load Row From Data Table
            FItemData* ItemData = ItemDataTable->FindRow<FItemData>(ItemName, TEXT("Load From Order Widget"));

            // Apply Image
            ItemWidget->SetItemIcon(ItemData->IconTexture);

            // And New Widget To The Wrap Box
            OrderWrapBox->AddChildToWrapBox(ItemWidget);



        }
    }


}


// Un-Used Function For Counting Up All The Orders
// New Function Makes Single Icons For Each Type Order Even For The Same Type Of Item

//void UOrderWidget::MakeOrder(FOrder NewOrder)
//{
//    // Make A New Map For Counting Amount Of The Same Item Types
//    TMap < FName, int32> ItemCounts;
//
//    // For Each Required Item
//    for (const FName& ItemName : NewOrder.RequiredItems)
//    {
//        // If Item Is Already Added 
//        // Apple Number >= 2 Added
//        if (ItemCounts.Contains(ItemName))
//        {
//            // Add +1 To Item Count
//            ItemCounts[ItemName]++;
//        }
//
//        // If Its The First Time This Item Is In The Order
//        // Apple Number 1 Is Loaded
//        else
//        {
//            // Add Current Item To ItemCounts
//            ItemCounts.Add(ItemName, 1);
//        }
//    }
//
//    for (const TPair<FName, int32>& ItemPair : ItemCounts)
//    {
//        FName ItemName = ItemPair.Key;
//        int32 Count = ItemPair.Value;
//
//        // Print for testing
//        UE_LOG(LogTemp, Warning, TEXT("Item: %s, Count: %d"), *ItemName.ToString(), Count);
//    }
//
//
//}
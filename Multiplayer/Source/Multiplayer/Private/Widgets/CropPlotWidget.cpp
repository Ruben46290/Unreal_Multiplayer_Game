// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CropPlotWidget.h"

void UCropPlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
}




void UCropPlotWidget::UpdateSeedUI(UTexture2D* IconTexture)
{

    if (!IconTexture) { return; }

    // Set Crop Type Image
    CropTypeImage->SetBrushFromTexture(IconTexture);

    // If Theres Infinite Seeds / Auto Regrowing On The Plot
    if (bAutoRegrowEnabled) {

        // Hide Seed Count Text
        SeedCountText->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UCropPlotWidget::UpdateSeedCount(int32 Seeds)
{
    // If Theres No Seeds Left
    if (Seeds == 0) {

        // If The Text Is Currently Visible
        if (SeedCountText->GetVisibility() == ESlateVisibility::Visible) {

            // Hide Seed Count Text
            SeedCountText->SetVisibility(ESlateVisibility::Hidden);

        }
    }

    // There Are Queued Seeds
    else {

        // If The Text Is Currently Hidden
        if (SeedCountText->GetVisibility() == ESlateVisibility::Hidden) {

            // Show Seed Count Text
            SeedCountText->SetVisibility(ESlateVisibility::Visible);

        }

        // Converts Int To A String, That Is Converted To Text (Probably a better way to do this)
        SeedCountText->SetText(FText::FromString(FString::FromInt(Seeds)));

    }
}



// Called On WaterTick()
void UCropPlotWidget::UpdateWaterUI(float WaterPercent)
{
    WaterProgressBar->SetPercent(WaterPercent);
}

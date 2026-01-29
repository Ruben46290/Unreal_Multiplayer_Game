// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CropPlotWidget.h"

void UCropPlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
}




void UCropPlotWidget::UpdateSeedUI(UTexture2D* IconTexture)
{
    // Set Crop Type Image
    CropTypeImage->SetBrushFromTexture(IconTexture);

    // If Theres Infinite Seeds / Auto Regrowing On The Plot
    if (bAutoRegrowEnabled) {

        // Hide Seed Count Text
        SeedCount->SetVisibility(ESlateVisibility::Hidden);
    }
}



// Called On WaterTick()
void UCropPlotWidget::UpdateWaterUI(float WaterPercent)
{
    WaterProgressBar->SetPercent(WaterPercent);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CropPlotWidget.h"

void UCropPlotWidget::NativeConstruct()
{
    Super::NativeConstruct();
}


void UCropPlotWidget::UpdateWaterUI(float WaterPercent)
{
    WaterProgressBar->SetPercent(WaterPercent);
}

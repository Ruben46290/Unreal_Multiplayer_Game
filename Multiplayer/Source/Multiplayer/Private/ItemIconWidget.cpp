// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemIconWidget.h"

void UItemIconWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UItemIconWidget::SetIcon(UTexture2D* IconTexture)
{
    if (IconImage && IconTexture)
    {
        IconImage->SetBrushFromTexture(IconTexture);
    }
}
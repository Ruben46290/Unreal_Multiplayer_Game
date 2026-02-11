// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OrderIconSlot.h"

void UOrderIconSlot::SetItemIcon(UTexture2D* IconTexture)
{
	if (!IconTexture) { return; }

	OrderIconImage->SetBrushFromTexture(IconTexture);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Ordering/CustomerWidget.h"



void UCustomerWidget::UpdateUI(float PatiencePercent)
{
	PatienceBar->SetPercent(PatiencePercent);
}

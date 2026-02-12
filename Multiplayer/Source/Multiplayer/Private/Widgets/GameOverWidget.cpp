// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameOverWidget.h"

void UGameOverWidget::SetupUI(float TotalScore)
{

	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("GameOverWidget Total Score %.2f")); }

	// Convert Score To String
	FString TotalScoreString = FString::Printf(TEXT("Total Score: $%.2f"), TotalScore);

	// Set Toal Score Text To The String That Was Just Made
	TotalScoreText->SetText(FText::FromString(TotalScoreString));
}

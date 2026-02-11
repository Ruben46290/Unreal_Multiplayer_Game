// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameHUD.h"
#include "MyGameState.h"
#include <Kismet/GameplayStatics.h>

void UGameHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// Get Game State
	AMyGameState* GS = Cast<AMyGameState>(UGameplayStatics::GetGameState(this));

	// If Game State Is Valid
	if (GS) {

		// Bind To OnTimeRemainingChanged Event Dispatcher
		GS->OnTimeRemainingChanged.AddDynamic(this, &UGameHUD::UpdateTimerText);

		// Bind Score Event Dispatcher
		GS->OnScoreChanged.AddDynamic(this, &UGameHUD::UpdateScoreText);
	}

}

void UGameHUD::UpdateTimerText(float TimeRemaining)
{
	// Get Minutes & Seconds
	int32 Minutes = FMath::FloorToInt(TimeRemaining / 60.0f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(TimeRemaining, 60.0f));

	// Set Timer Text
	FString TimerString = FString::Printf(TEXT("%2d:%02d"), Minutes, Seconds);
	TimerText->SetText(FText::FromString(TimerString));
}



void UGameHUD::UpdateScoreText(float NewScore)
{

	FString ScoreString = FString::Printf(TEXT("$%.2f"),NewScore);
	ScoreText->SetText(FText::FromString(ScoreString));
}

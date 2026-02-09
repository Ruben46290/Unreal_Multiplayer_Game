// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameStartWidget.h"
#include "MyPlayerController.h"

void UGameStartWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Play FadeIn Animation
	PlayFadeInAnimation();

	// Set Status Text
	UpdateStatusText(TEXT("Waiting for other players to load..."));
}

//void UGameStartWidget::SetPlayerReady()
//{
//	bIsReady = true;
//}

void UGameStartWidget::StartCountdown()
{
	// Clear Status Text
	UpdateStatusText(TEXT(""));

	// Start Countdown From 3
	CountdownValue = 3;
	ShowCountdown(CountdownValue);

	// Tick Every Second
	GetWorld()->GetTimerManager().SetTimer(CountdownTimer, this, &UGameStartWidget::OnCountdownTick, 1.0f, true);
}

void UGameStartWidget::OnCountdownTick()
{
	// -1 From Countdown Value
	CountdownValue--;

	// Is The Countdown Not Done?
	if (CountdownValue > 0)
	{
		// Update Countdown Text
		ShowCountdown(CountdownValue);
	}

	// Countdown Is Done
	else
	{
		// Show Countdown As 0 - Hides The Countdown Text In The Blueprint
		ShowCountdown(CountdownValue);

		// Countdown Finished - Stop Timer
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimer);

		// Tell the server we're ready to start
		AMyPlayerController* PC = Cast<AMyPlayerController>(GetOwningPlayer());
		if (PC)
		{
			PC->Server_NotifyCountdownFinished();
		}
		else {
			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GameStartWidget - MyPlayerController Cast Invalid")); }
		}

		// Hide Widget 1 Second After Showing "GO"
		FTimerHandle HideTimer;
		GetWorld()->GetTimerManager().SetTimer(HideTimer, [this]()
			{
				HideWidget();
				RemoveFromParent();
			}, 1.0f, false);

	}
}

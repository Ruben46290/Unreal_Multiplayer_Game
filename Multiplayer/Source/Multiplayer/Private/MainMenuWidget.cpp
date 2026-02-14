// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	// Get reference to game instance
	GameInstanceRef = Cast<UMyGameInstance>(GetGameInstance());

	// Bind button click events
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinButtonClicked);
	}
}

void UMainMenuWidget::OnHostButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Host button clicked!"));

	if (GameInstanceRef)
	{
		GameInstanceRef->HostSession();
	}
}

void UMainMenuWidget::OnJoinButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Join button clicked!"));

	if (GameInstanceRef)
	{
		// First find sessions, then join
		GameInstanceRef->FindSessions();

		// In a real implementation, you'd wait for the search to complete
		// and show a list of sessions. For now, we'll add a delay
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (GameInstanceRef)
				{
					GameInstanceRef->JoinSession();
				}
			}, 5.0f, false); // Wait 1 second for search to complete
	}
}

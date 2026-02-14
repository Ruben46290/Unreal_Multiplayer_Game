// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	// Get reference to game instance
	GameInstanceRef = Cast<UMyGameInstance>(GetGameInstance());

	// Bind button click events
	if (HostLANButton)
	{
		HostLANButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostLANButtonClicked);
	}

	if (HostOnlineButton)
	{
		HostOnlineButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostOnlineButtonClicked);
	}

	if (JoinLANButton)
	{
		JoinLANButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinLANButtonClicked);
	}

	if (JoinOnlineButton)
	{
		JoinOnlineButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinOnlineButtonClicked);
	}
}

void UMainMenuWidget::OnHostLANButtonClicked()
{
	if (GameInstanceRef)
	{
		GameInstanceRef->HostLANSession();
	}
}

void UMainMenuWidget::OnHostOnlineButtonClicked()
{
	if (GameInstanceRef)
	{
		GameInstanceRef->HostOnlineSession();
	}
}

void UMainMenuWidget::OnJoinLANButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Join button clicked!"));

	if (GameInstanceRef)
	{
		// First find sessions, then join
		GameInstanceRef->FindLANSessions();

		// In a real implementation, you'd wait for the search to complete
		// and show a list of sessions. For now, we'll add a delay
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (GameInstanceRef)
				{
					GameInstanceRef->JoinSession();
				}
			}, 1.0f, false); // Wait 1 second for search to complete
	}
}

void UMainMenuWidget::OnJoinOnlineButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Join button clicked!"));

	if (GameInstanceRef)
	{
		// First find sessions, then join
		GameInstanceRef->FindOnlineSessions();

		// In a real implementation, you'd wait for the search to complete
		// and show a list of sessions. For now, we'll add a delay
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
			{
				if (GameInstanceRef)
				{
					GameInstanceRef->JoinSession();
				}
			}, 1.0f, false); // Wait 1 second for search to complete
	}
}



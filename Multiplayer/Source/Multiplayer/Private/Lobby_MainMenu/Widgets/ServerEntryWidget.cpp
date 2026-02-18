// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/Widgets/ServerEntryWidget.h"
#include "Lobby_MainMenu/Widgets/PlayScreenWidget.h"

void UServerEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind Button Click Event
	if (EntryButton)
	{
		EntryButton->OnClicked.AddDynamic(this, &UServerEntryWidget::OnButtonClicked);
	}
}

void UServerEntryWidget::SetServerInfo(FServerInfo Info)
{
	// Save Server Info
	ServerInfo = Info;

	// Format Player & Server Type Text
	FString Players = FString::Printf(TEXT("%d/%d"), Info.CurrentPlayers, Info.MaxPlayers);
	FString Type = Info.bIsLAN ? TEXT("LAN") : TEXT("Online");

	// Update Visuals In Blueprints
	UpdateServerEntry(Info.ServerName, Players, Type, Info.bIsPasswordProtected);
}

void UServerEntryWidget::OnButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("ServerEntryWidget Button Pressed"));
	// Tell the play screen this entry was clicked
	if (PlayScreenRef)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("ServerEntryWidget Button Pressed With Valid Refrence"));
		PlayScreenRef->OnServerEntryClicked(ServerInfo);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("! ServerEntryWidget Play Screen Refrence Invalid !"));
	}
}


void UServerEntryWidget::SetPlayScreen(UPlayScreenWidget* PlayScreen)
{
	PlayScreenRef = PlayScreen;
}


	// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/Widgets/PlayScreenWidget.h"

void UPlayScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Get Game Instance
	GameInstanceRef = Cast<UMyGameInstance>(GetGameInstance());

	// Bind Button Click Events
	//if (HostButton)
	//{
	//	HostButton->OnClicked.AddDynamic(this, &UPlayScreenWidget::OnHostButtonClicked);
	//}
	//if (RefreshButton)
	//{
	//	RefreshButton->OnClicked.AddDynamic(this, &UPlayScreenWidget::OnRefreshButtonClicked);
	//}
	//if (BackButton)
	//{
	//	BackButton->OnClicked.AddDynamic(this, &UPlayScreenWidget::OnBackButtonClicked);
	//}


	if (GameInstanceRef)
	{
		// Bind To Event Dispatcher For Sessions Found
		GameInstanceRef->OnSessionsFound.AddDynamic(this, &UPlayScreenWidget::OnSessionsFound);

		// Auto Refresh Server List
		bool bIsLan = LANCheckBox ? LANCheckBox->IsChecked() : false;
		GameInstanceRef->RefreshServerList(bIsLan);

		// Hide No Servers Found Text Until We Know If There Are Servers Or Not
		if (NoServersText)
		{
			NoServersText->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}


void UPlayScreenWidget::OnHostButtonClicked()
{
	if (HostPopupWidgetClass)
	{
		// Create Host Popup Widget
		UHostPopupWidget* HostPopup = CreateWidget<UHostPopupWidget>(GetWorld(), HostPopupWidgetClass);

		// If Widget Valid
		if (HostPopup)
		{
			// Add To Viewport
			HostPopup->AddToViewport();
		}
	}
}

void UPlayScreenWidget::OnRefreshButtonClicked()
{
	if (GameInstanceRef) {

		bool bIsLan = LANCheckBox ? LANCheckBox->IsChecked() : false;
		GameInstanceRef->RefreshServerList(bIsLan);

		// Clear Server List (Until New One Is Populated)
		if (ServerListScrollBox) {
			ServerListScrollBox->ClearChildren();
		}


		if (NoServersText) {
			NoServersText->SetVisibility(ESlateVisibility::Hidden);
		}

	}
}

void UPlayScreenWidget::OnBackButtonClicked()
{

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Back Button Pressed"));

	// Unbind From Event Dispatcher For Sessions Found
	if (GameInstanceRef) {
		GameInstanceRef->OnSessionsFound.RemoveDynamic(this, &UPlayScreenWidget::OnSessionsFound);
	}

	// Boradcast Event Dispatcher To Tell Main Menu Widget To Switch Back To Main Menu Screen
	OnBackPressed.Broadcast();
}

void UPlayScreenWidget::OnSessionsFound(const TArray<FServerInfo>& ServerList)
{
	PopulateServerList(ServerList);
}

void UPlayScreenWidget::PopulateServerList(const TArray<FServerInfo>& ServerList)
{
	if (!ServerListScrollBox) return;

	// Clear Scroll Box
	ServerListScrollBox->ClearChildren();

	// No Servers Found
	if (ServerList.Num() == 0)
	{
		// Show No Servers Found Text
		if (NoServersText)
		{
			NoServersText->SetVisibility(ESlateVisibility::Visible);
		}
		return;
	}

	// Hide No Servers Found Text
	if (NoServersText)
	{
		NoServersText->SetVisibility(ESlateVisibility::Hidden);
	}

	// For Each Found Server
	for (const FServerInfo& Info : ServerList)
	{
		if (ServerEntryWidgetClass)
		{
			// Create Server Entry Widget
			UServerEntryWidget* Entry = CreateWidget<UServerEntryWidget>(GetOwningPlayer(), ServerEntryWidgetClass);
			if (Entry)
			{
				Entry->SetServerInfo(Info);
				ServerListScrollBox->AddChild(Entry);
				Entry->SetPlayScreen(this);
			}
		}
	}
}



void UPlayScreenWidget::OnServerEntryClicked(FServerInfo ServerInfo)
{
	if (ServerInfo.bIsPasswordProtected)
	{
		// Show password prompt
		if (PasswordPromptWidgetClass)
		{
			UPasswordPrmoptWidget* PasswordPrompt = CreateWidget<UPasswordPrmoptWidget>(GetOwningPlayer(), PasswordPromptWidgetClass);
			if (PasswordPrompt)
			{
				PasswordPrompt->SetTargetServer(ServerInfo);
				PasswordPrompt->AddToViewport(10);
			}
		}
	}
	else
	{
		// Join directly
		if (GameInstanceRef)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PlayScreenWidget Join Session Called"));
			GameInstanceRef->JoinSessionByIndex(ServerInfo.SearchResultIndex, TEXT(""));
		}
	}
}	
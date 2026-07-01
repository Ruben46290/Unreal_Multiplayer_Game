// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/Widgets/HostPopupWidget.h"
#include <MyPlayerController.h>




void UHostPopupWidget::NativeConstruct()
{
	// Get Game Instance
	GameInstanceRef = Cast<UMyGameInstance>(GetGameInstance());

	// Bind Create Cutton Click Event
	if (CreateButton)
	{
		CreateButton->OnClicked.AddDynamic(this, &UHostPopupWidget::OnCreateClicked);
	}

	// Bind Cancel Button Click Event
	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UHostPopupWidget::OnCancelClicked);
	}
}



void UHostPopupWidget::OnCreateClicked()
{
	// Get Values From Input Fields
	FString ServerName = ServerNameInput ? ServerNameInput->GetText().ToString() : TEXT("My Server");
	FString Password = PasswordInput ? PasswordInput->GetText().ToString() : TEXT("");
	bool bIsLAN = LANCheckBox ? LANCheckBox->IsChecked() : false;

	// If No Server Name Was Set
	if (ServerName.IsEmpty()){

		// Set Defualt Server Name
		ServerName = TEXT("My Server");
	}
	
	// Host The Server
	GameInstanceRef->HostSessionWithSettings(ServerName, bIsLAN, Password);

	// Get Player Controller & Show Loading Screen
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->Client_ShowLoadingScreen();
	}

	// Close The Popup
	RemoveFromParent();
}




void UHostPopupWidget::OnCancelClicked()
{
	// Close The Popup
	RemoveFromParent();
}
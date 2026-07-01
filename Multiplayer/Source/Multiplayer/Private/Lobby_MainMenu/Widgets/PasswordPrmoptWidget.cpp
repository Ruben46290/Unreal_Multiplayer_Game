// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/Widgets/PasswordPrmoptWidget.h"
#include <MyPlayerController.h>

// Spelling Mistake - Should Be PasswordPromptWidget 

void UPasswordPrmoptWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameInstanceRef = Cast<UMyGameInstance>(GetGameInstance());

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.AddDynamic(this, &UPasswordPrmoptWidget::OnConfirmClicked);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UPasswordPrmoptWidget::OnCancelClicked);
	}
}

void UPasswordPrmoptWidget::SetTargetServer(FServerInfo Info)
{
	// Save Target Server Info
	TargetServer = Info;
}

void UPasswordPrmoptWidget::OnConfirmClicked()
{
	if (!GameInstanceRef) return;

	// Get Entered Password
	FString EnteredPassword = PasswordInput ? PasswordInput->GetText().ToString() : TEXT("");

	// Dismiss The Prompt (If Password Is Wrong, ShowWrongPassword() Will Be Called From GameInstance And This Widget Will Stay Open)
	RemoveFromParent();

	// Get Player Controller & Show Loading Screen
	AMyPlayerController* PC = Cast<AMyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->Client_ShowLoadingScreen();
	}

	// Join The Server
	GameInstanceRef->JoinSessionByIndex(TargetServer.SearchResultIndex, EnteredPassword);
}


void UPasswordPrmoptWidget::OnCancelClicked()
{
	// Destroy The Widget
	RemoveFromParent();
}
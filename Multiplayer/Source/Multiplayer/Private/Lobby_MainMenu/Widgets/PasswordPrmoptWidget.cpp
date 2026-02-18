// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/Widgets/PasswordPrmoptWidget.h"

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

	// Join The Server
	GameInstanceRef->JoinSessionByIndex(TargetServer.SearchResultIndex, EnteredPassword);

	// Dismiss The Prompt (If Password Is Wrong, ShowWrongPassword() Will Be Called From GameInstance And This Widget Will Stay Open)
	RemoveFromParent();
}

void UPasswordPrmoptWidget::OnCancelClicked()
{
	// Destroy The Widget
	RemoveFromParent();
}
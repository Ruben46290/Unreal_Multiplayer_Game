// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "MyGameInstance.h"
#include "Lobby_MainMenu/Widgets/ServerEntryWidget.h"
#include "Lobby_MainMenu/Widgets/HostPopupWidget.h"
#include "Lobby_MainMenu/Widgets/PasswordPrmoptWidget.h"
#include "PlayScreenWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackPressed);

UCLASS()
class MULTIPLAYER_API UPlayScreenWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// Called by ServerEntryWidget when clicked
	void OnServerEntryClicked(FServerInfo ServerInfo);

protected:
	// Server list scroll box
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ServerListScrollBox;

	// Event Dispatcher For When Back Button Is Pressed - Bound In Main Menu Widget To Return To Main Menu Screen
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FOnBackPressed OnBackPressed;

	// LAN / Online toggle
	UPROPERTY(meta = (BindWidget))
	UCheckBox* LANCheckBox;

	// No servers found text
	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoServersText;

	// Widget classes to spawn
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UServerEntryWidget> ServerEntryWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UHostPopupWidget> HostPopupWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UPasswordPrmoptWidget> PasswordPromptWidgetClass;

	// Button click handlers
	UFUNCTION(BlueprintCallable, Category = "Button")
	void OnHostButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Button")
	void OnRefreshButtonClicked();

	UFUNCTION(BlueprintCallable, Category = "Button")
	void OnBackButtonClicked();

	// Called when sessions are found
	UFUNCTION()
	void OnSessionsFound(const TArray<FServerInfo>& ServerList);

	// Populate the server list
	void PopulateServerList(const TArray<FServerInfo>& ServerList);

private:
	UMyGameInstance* GameInstanceRef;
};

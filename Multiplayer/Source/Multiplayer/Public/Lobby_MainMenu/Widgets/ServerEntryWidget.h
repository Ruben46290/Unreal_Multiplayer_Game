// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <MyGameInstance.h>
#include "Components/Button.h"
#include "ServerEntryWidget.generated.h"


UCLASS()
class MULTIPLAYER_API UServerEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:


	// Set Server Info
	UFUNCTION(BlueprintCallable, Category = "Server")
	void SetServerInfo(FServerInfo Info);

	// Blueprint Event To Update Visuals With Server Data Info
	UFUNCTION(BlueprintImplementableEvent, Category = "Server")
	void UpdateServerEntry(const FString& ServerName, const FString& Players, const FString& Type, bool bIsPasswordProtected);

	// Set Refrence To Play Screen Widget
	UFUNCTION()
	void SetPlayScreen(UPlayScreenWidget* PlayScreen);

protected:
	// Store server info for when player clicks
	FServerInfo ServerInfo;

	// Refrence To Play Screen
	UPROPERTY()
	UPlayScreenWidget* PlayScreenRef;

	UPROPERTY(meta = (BindWidget))
	UButton* EntryButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnButtonClicked();
};

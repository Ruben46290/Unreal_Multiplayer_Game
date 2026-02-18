// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "MyGameInstance.h"
#include "HostPopupWidget.generated.h"


UCLASS()
class MULTIPLAYER_API UHostPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	// Input fields
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ServerNameInput;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* PasswordInput;

	// LAN / Online toggle
	UPROPERTY(meta = (BindWidget))
	UCheckBox* LANCheckBox;

	// Buttons
	UPROPERTY(meta = (BindWidget))
	UButton* CreateButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	UFUNCTION()
	void OnCreateClicked();

	UFUNCTION()
	void OnCancelClicked();

private:
	UMyGameInstance* GameInstanceRef;
};

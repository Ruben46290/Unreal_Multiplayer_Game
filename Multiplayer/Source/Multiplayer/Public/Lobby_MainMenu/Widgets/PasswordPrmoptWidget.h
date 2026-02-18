// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "MyGameInstance.h"
#include "PasswordPrmoptWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API UPasswordPrmoptWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// Set which server we're trying to join
	void SetTargetServer(FServerInfo Info);

protected:
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* PasswordInput;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelButton;

	// Show Wrong Password Animation - Blueprint Event
	UFUNCTION(BlueprintImplementableEvent, Category = "Password")
	void ShowWrongPassword();

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();

private:
	FServerInfo TargetServer;
	UMyGameInstance* GameInstanceRef;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyGameState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../MultiplayerGameMode.h"
#include <MyGameInstance.h>
#include <EnhancedInputSubsystems.h>

AMyPlayerController::AMyPlayerController()
{
	// Load 
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/Widgets/WBP_GameOver.WBP_GameOver_C"));
	if (WidgetClass.Succeeded())
	{
		LevelCompleteWidgetClass = WidgetClass.Class;
	}
	else {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("MyPlayerController Failed To Load GameOver Widget")); }
	}
}


void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set Input Mode
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	// Make A Timer For A Small Delay Before Connecting To Game State
	FTimerHandle BindTimer;
	GetWorldTimerManager().SetTimer(BindTimer, this, &AMyPlayerController::BindToGameState, 0.25f, false);

	// Get GameInstance
	if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
	{
		// Is The Game SinglePlayer?
		// Singleplayer
		if (GI->bIsSingleplayer)
		{
			
			ShowGameStartWidget();

			// Get GameMode
			AMultiplayerGameMode* GM = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode());

			GM->CheckAllPlayersLoaded();
		}

		// Multiplayer
		else {
			ShowGameStartWidget();

			// Connect To The Gamemode To Connect To The Server & Start The Game After A Small Delay
			FTimerHandle NotifyTimer;
			GetWorld()->GetTimerManager().SetTimer(NotifyTimer, [this]()
				{
					AMultiplayerGameMode* GM = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode());
					if (GM)
					{
						GM->CheckAllPlayersLoaded();
					}
				}, 2.0f, false);
		}
	}
}

void AMyPlayerController::SetGameplayInputEnabled(bool bEnabled)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!Subsystem || !DefaultMappingContext) return;

	if (bEnabled)
	{
		if (!Subsystem->HasMappingContext(DefaultMappingContext))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	else
	{
		Subsystem->RemoveMappingContext(DefaultMappingContext);
	}
}

// * * * * * * * * * * Game Start / Loading * * * * * * * * * *

// Make Starting Widget
void AMyPlayerController::ShowGameStartWidget()
{
	if (GameStartWidgetClass && IsLocalController())
	{
		GameStartWidget = CreateWidget<UGameStartWidget>(this, GameStartWidgetClass);
		if (GameStartWidget)
		{
			GameStartWidget->AddToViewport(999);
		}
	}

	// Disable Input
	SetGameplayInputEnabled(false);
}

void AMyPlayerController::Server_NotifyCountdownFinished_Implementation()
{
	AMultiplayerGameMode* GM = Cast<AMultiplayerGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->StartGameplay();
	}
}

// Start Countdown On Starting Widget
void AMyPlayerController::Client_StartGameCountdown_Implementation()
{
	if (GameStartWidget)
	{
		GameStartWidget->StartCountdown();
	}
}

// Bind To Game State Event Dispathcers
void AMyPlayerController::BindToGameState()
{
	// Get Game State
	AMyGameState* GS = GetWorld()->GetGameState<AMyGameState>();
	if (GS) {

		// Bind To Level Completed Event Dispatcher
		GS->OnLevelComplete.AddDynamic(this, &AMyPlayerController::ShowLevelCompleteScreen);
	}
}

void AMyPlayerController::ShowLevelCompleteScreen()
{
	// Only Run On Local Machine
	if (!IsLocalController()) { return; }

	// If Widget Class Was Set Properly
	if (LevelCompleteWidgetClass) {

		// Create A Widget Using The Widget Class
		LevelCompleteWidget = CreateWidget<UGameOverWidget>(this, LevelCompleteWidgetClass);

		// If Widget Valid
		if (LevelCompleteWidget)
		{
			// Get Game State
			AMyGameState* GS = GetWorld()->GetGameState<AMyGameState>();

			// Set Widget Variables
			LevelCompleteWidget->SetupUI(GS->CurrentScore, GS->LastMilestoneIndex, GS->TimeRemaining);

			// Show Widget
			LevelCompleteWidget->AddToViewport(10); // High Z-order to appear on top
		}

		// Unlock and show the mouse cursor
		bShowMouseCursor = true;

		// Switch input mode so UI can receive mouse/click input
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(LevelCompleteWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(true);
		SetInputMode(InputMode);

		// Disable Input
		SetGameplayInputEnabled(false);
	}

}

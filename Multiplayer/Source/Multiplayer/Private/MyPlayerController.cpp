// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "MyGameState.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../MultiplayerGameMode.h"

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

	ShowGameStartWidget();

	// Set Input Mode
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	// Make A Timer For A Small Delay Before Connecting To Game State
	FTimerHandle BindTimer;
	GetWorldTimerManager().SetTimer(BindTimer, this, &AMyPlayerController::BindToGameState, 0.25f, false);

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
			LevelCompleteWidget->SetupUI(GS->CurrentScore);

			// Show Widget
			LevelCompleteWidget->AddToViewport(10); // High Z-order to appear on top
		}

		// Pause Game
		//UGameplayStatics::SetGamePaused(GetWorld(), true);
	}

}

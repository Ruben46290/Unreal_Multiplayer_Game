// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include <OnlineSessionSettings.h>
#include <Online/OnlineSessionNames.h>
#include <Kismet/GameplayStatics.h>
#include "ILoadingScreenModule.h"

UMyGameInstance::UMyGameInstance()
{
}

void UMyGameInstance::Init()
{
	Super::Init();

	// Get the online subsystem (NULL subsystem for LAN)
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	if (OnlineSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Online Subsystem found: %s"), *OnlineSubsystem->GetSubsystemName().ToString());

		// Get the session interface
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if (OnlineSessionInterface.IsValid())
		{
			// Bind delegates for session callbacks
			OnlineSessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnCreateSessionComplete);
			OnlineSessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMyGameInstance::OnFindSessionsComplete);
			OnlineSessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMyGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Online Subsystem not found!"));
	}
}

void UMyGameInstance::HostLANSession()
{
	bIsLAN = true;

	if (!OnlineSessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	// Check if a session already exists and destroy it
	FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	// Create session settings
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	// Configure session settings
	SessionSettings->bIsLANMatch = true; // LAN match (NULL subsystem)
	SessionSettings->NumPublicConnections = 2; // Max 4 players
	SessionSettings->bShouldAdvertise = true; // Advertise so other players can find it
	SessionSettings->bUsesPresence = false; // Don't use presence (not needed for NULL subsystem)
	SessionSettings->bAllowJoinInProgress = true; // Allow joining while game is running
	SessionSettings->bAllowJoinViaPresence = false; // Not needed for NULL subsystem
	SessionSettings->bUseLobbiesIfAvailable = false; // Not using lobbies

	// Get the first local player controller
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	// Create the session
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);

	UE_LOG(LogTemp, Warning, TEXT("Creating session..."));
}

void UMyGameInstance::HostOnlineSession()
{
	bIsLAN = false;

	if (!OnlineSessionInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	// Online session settings - different from LAN!
	SessionSettings->bIsLANMatch = false;  // Not LAN
	SessionSettings->NumPublicConnections = 4;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bUsesPresence = true;  // EOS uses presence
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bAllowJoinViaPresence = true;  // EOS needs this
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->bUsesStats = true;
	SessionSettings->bUseLobbiesVoiceChatIfAvailable = false;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Creating Online session..."));
}

void UMyGameInstance::FindLANSessions()
{
	if (!OnlineSessionInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, false, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Searching for LAN sessions..."));
}

void UMyGameInstance::FindOnlineSessions()
{
	if (!OnlineSessionInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;  // Search online
	SessionSearch->MaxSearchResults = 50;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Searching for Online sessions..."));
}


void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session created successfully!"));

		// Travel to the game map (server travel)
		GetWorld()->ServerTravel("/Game/Lobby_MainMenu/Maps/LobbyMap?listen"); // Change to your game map path
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create session!"));
	}
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session search complete! Found %d sessions"), SessionSearch->SearchResults.Num());

		// Log all found sessions
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("Session %d: %s"), i, *SessionSearch->SearchResults[i].GetSessionIdStr());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Session search failed!"));
	}
}

void UMyGameInstance::JoinSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	// Destroy any existing session first
	FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Destroying existing session before joining..."));
	}

	if (!SessionSearch.IsValid() || SessionSearch->SearchResults.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No sessions found to join!"));
		return;
	}
	// Get the first local player controller
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// Join the first session found
	OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSearch->SearchResults[0]);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Attempting to join session..."));
}

void UMyGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Successfully joined session!"));

		// Get the connection string from the session
		FString ConnectInfo;
		if (OnlineSessionInterface->GetResolvedConnectString(SessionName, ConnectInfo))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connect string: %s"), *ConnectInfo));

			// Get player controller and execute console command to travel
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Executing travel command..."));
				PlayerController->ConsoleCommand(FString::Printf(TEXT("open %s"), *ConnectInfo));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No player controller!"));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to get connect string!"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to join session!"));
	}
}


void UMyGameInstance::StartSession()
{
}

void UMyGameInstance::ManuallyShowLoadingScreen()
{
	ILoadingScreenModule& LoadingScreenModule = FModuleManager::LoadModuleChecked<ILoadingScreenModule>("LoadingScreen");
	LoadingScreenModule.StartInGameLoadingScreen();
}
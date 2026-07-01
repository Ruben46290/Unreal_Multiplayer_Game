// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include <OnlineSessionSettings.h>
#include <Online/OnlineSessionNames.h>
#include <Kismet/GameplayStatics.h>
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Blueprint/UserWidget.h"

const FName UMyGameInstance::SERVER_NAME_KEY = FName("ServerName");
const FName UMyGameInstance::PASSWORD_KEY = FName("Password");
const FName UMyGameInstance::IS_PASSWORD_PROTECTED_KEY = FName("IsPasswordProtected");

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


// * * * * * * * * * * Server Hosting & Joining * * * * * * * * * *

void UMyGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green, TEXT("Session created successfully!"));

		// Travel to the game map (server travel)
		GetWorld()->ServerTravel("/Game/Lobby_MainMenu/Maps/LobbyMap?listen"); // Change to your game map path
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Failed to create session!"));
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

void UMyGameInstance::HostSessionWithSettings(FString ServerName, bool IsLAN, FString Password)
{
	if (!OnlineSessionInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	// Destroy existing session if there is one
	FNamedOnlineSession* ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	// Set LAN or Online settings
	SessionSettings->bIsLANMatch = IsLAN;
	SessionSettings->NumPublicConnections = 2;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bUseLobbiesIfAvailable = !IsLAN;
	SessionSettings->bUsesPresence = false;
	SessionSettings->bAllowJoinViaPresence = false;

	// Setup Bucket Name For Searching To Work
	SessionSettings->Set(FName(TEXT("GameLobbyTag")), FString(TEXT("MyGameLobbies")), EOnlineDataAdvertisementType::ViaOnlineService);

	// Store server name in session settings
	SessionSettings->Set(SERVER_NAME_KEY, ServerName, EOnlineDataAdvertisementType::ViaOnlineService);

	// Store password info
	bool bHasPassword = !Password.IsEmpty();
	SessionSettings->Set(IS_PASSWORD_PROTECTED_KEY, bHasPassword, EOnlineDataAdvertisementType::ViaOnlineService);

	// Only store password if there is one
	if (bHasPassword)
	{
		SessionSettings->Set(PASSWORD_KEY, Password, EOnlineDataAdvertisementType::ViaOnlineService);
		CurrentPassword = Password;
	}

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);

	// Set To A Multiplayer Game
	bIsSingleplayer = false;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Creating session: %s"), *ServerName));
}

void UMyGameInstance::RefreshServerList(bool bSearchLAN)
{
	if (!OnlineSessionInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = bSearchLAN;
	SessionSearch->MaxSearchResults = 50;

	//SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, false, EOnlineComparisonOp::Equals);
	//SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, !bSearchLAN, EOnlineComparisonOp::Equals);
	// Temporarily set both to true to ensure we find sessions regardless of type
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);


	SessionSearch->QuerySettings.Set(FName(TEXT("GameLobbyTag")), FString(TEXT("MyGameLobbies")), EOnlineComparisonOp::Equals);


	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Searching for sessions..."));
}

void UMyGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	TArray<FServerInfo> ServerList;

	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("OnFindSessionsComplete called"));

	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
			FString::Printf(TEXT("Found %d sessions"), SessionSearch->SearchResults.Num()));

		// Loop through all found sessions and build server info
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++)
		{
			FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[i];

			FServerInfo Info;

			// Get server name from session settings
			FString ServerName;
			Result.Session.SessionSettings.Get(SERVER_NAME_KEY, ServerName);
			Info.ServerName = ServerName.IsEmpty() ? TEXT("Unnamed Server") : ServerName;

			// Get password protection status
			bool bIsPasswordProtected = false;
			Result.Session.SessionSettings.Get(IS_PASSWORD_PROTECTED_KEY, bIsPasswordProtected);
			Info.bIsPasswordProtected = bIsPasswordProtected;

			// Get player counts
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;

			// LAN or Online
			Info.bIsLAN = Result.Session.SessionSettings.bIsLANMatch;

			// Store index for joining later
			Info.SearchResultIndex = i;

			ServerList.Add(Info);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Session search failed!"));
	}

	// Fire delegate so UI can update (even if empty - will show "No servers found")
	OnSessionsFound.Broadcast(ServerList);
}

void UMyGameInstance::JoinSessionByIndex(int32 Index, FString Password)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Game Instance Join Session Called"));
	if (!OnlineSessionInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("OnlineSessionInterface is not valid!"));
		return;
	}

	if (!SessionSearch.IsValid() || !SessionSearch->SearchResults.IsValidIndex(Index))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid session index!"));
		return;
	}

	FOnlineSessionSearchResult& Result = SessionSearch->SearchResults[Index];

	// Validate password if session is password protected
	bool bIsPasswordProtected = false;
	Result.Session.SessionSettings.Get(IS_PASSWORD_PROTECTED_KEY, bIsPasswordProtected);

	if (bIsPasswordProtected)
	{
		FString ServerPassword;
		Result.Session.SessionSettings.Get(PASSWORD_KEY, ServerPassword);

		if (Password != ServerPassword)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Wrong password!"));
			return;
		}
	}

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Joining session..."));
}


// * * * * * * * * * * * Skin Customization * * * * * * * * * *

void UMyGameInstance::SavePlayerSkin(const FString& PlayerName, int32 SkinIndex)
{
	PlayerSkinSelections.Add(PlayerName, SkinIndex);
	//UE_LOG(LogTemp, Warning, TEXT("GameInstance saved skin %d for player %s"), SkinIndex, *PlayerName);

}

int32 UMyGameInstance::GetPlayerSkin(const FString& PlayerName)
{

	//UE_LOG(LogTemp, Warning, TEXT("GameInstance looking up skin for: '%s'"), *PlayerName);

	if (int32* SkinIndex = PlayerSkinSelections.Find(PlayerName))
	{
		//UE_LOG(LogTemp, Warning, TEXT("GameInstance found skin %d for player %s"), *SkinIndex, *PlayerName);
		return *SkinIndex;
	}
	//UE_LOG(LogTemp, Warning, TEXT("GameInstance found no skin for player %s, defaulting to 0"), *PlayerName);
	return 0;
}


// * * * * * * * * * * * Online Servies * * * * * * * * * * *
void UMyGameInstance::LoginToEOS()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Online subsystem invalid!"));
		return;
	}

	IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
	if (!IdentityInterface.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("Identity interface invalid!"));
		return;
	}

	FOnlineAccountCredentials Credentials;
	Credentials.Type = TEXT("AccountPortal");
	Credentials.Id = TEXT("");
	Credentials.Token = TEXT("");

	IdentityInterface->OnLoginCompleteDelegates[0].AddUObject(this, &UMyGameInstance::OnLoginComplete);
	IdentityInterface->Login(0, Credentials);
}

void UMyGameInstance::OnLoginComplete(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	// Call Login Complete Event Dispatcher 
	OnEOSLoginComplete.Broadcast(bWasSuccessful);

	if (bWasSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Green, TEXT("EOS login successful!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, FString::Printf(TEXT("EOS login failed: %s"), *Error));
	}
}

bool UMyGameInstance::IsLoggedInToEOS()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return false;

	IOnlineIdentityPtr IdentityInterface = Subsystem->GetIdentityInterface();
	if (!IdentityInterface.IsValid()) return false;

	return IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

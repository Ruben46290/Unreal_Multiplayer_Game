// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h" // Needs OnlineSubsystem To Be Added To The Build.cs
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstance.generated.h"

// Struct For Server Infomation
USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	FString HostName;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadOnly)
	bool bIsLAN;

	UPROPERTY(BlueprintReadOnly)
	bool bIsPasswordProtected;

	// Index in search results array
	int32 SearchResultIndex;
};

// Event Dispatcher For Sessions Found
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionsFound, const TArray<FServerInfo>&, ServerList);

UCLASS()
class MULTIPLAYER_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:

	UMyGameInstance();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void HostLANSession();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void HostOnlineSession();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void FindLANSessions();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void FindOnlineSessions();

	void JoinSession();
protected:
	// Online session interface pointer
	IOnlineSessionPtr OnlineSessionInterface;

	// Delegate handles for session callbacks
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	// Session search results
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// Callback functions for session operations
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);




	bool bIsLAN;


public:
	// Delegate that fires when sessions are found (UI binds to this)
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer")
	FOnSessionsFound OnSessionsFound;

	// Updated host function that takes parameters
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void HostSessionWithSettings(FString ServerName, bool bIsLAN, FString Password);

	// Join a specific session by index
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void JoinSessionByIndex(int32 Index, FString Password);

	// Refresh server list
	UFUNCTION(BlueprintCallable, Category = "Multiplayer")
	void RefreshServerList(bool bSearchLAN);

private:
	// Store password for validation when joining
	FString CurrentPassword;

	// Settings keys for session data
	static const FName SERVER_NAME_KEY;
	static const FName PASSWORD_KEY;
	static const FName IS_PASSWORD_PROTECTED_KEY;
};

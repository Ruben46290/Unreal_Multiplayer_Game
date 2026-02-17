// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h" // Needs OnlineSubsystem To Be Added To The Build.cs
#include "Interfaces/OnlineSessionInterface.h"
#include "MyGameInstance.generated.h"


/**
 * 
 */
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


	// Helper function to start the game after hosting
	void StartSession();

	bool bIsLAN;

};

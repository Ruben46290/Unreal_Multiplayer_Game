// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;

public:

    UPROPERTY(ReplicatedUsing = OnRep_SelectedSkinIndex)
    int32 SelectedSkinIndex = 0;

    UFUNCTION(Server, Reliable)
    void ServerSetSkin(int32 NewSkinIndex);

    UFUNCTION()
    void OnRep_SelectedSkinIndex();

    UFUNCTION(NetMulticast, Reliable)
    void MulticastSaveSkin(const FString& PlayerName, int32 SkinIndex);

};

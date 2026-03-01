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

   // virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutProps) const override;
    virtual void CopyProperties(APlayerState* PlayerState) override;
    virtual void OverrideWith(APlayerState* PlayerState) override;
};

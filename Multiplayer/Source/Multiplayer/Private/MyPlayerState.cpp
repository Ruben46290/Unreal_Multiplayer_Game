// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Lobby_MainMenu/LobbyCharacter.h"



void AMyPlayerState::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("PlayerState BeginPlay - SkinIndex: %d"), SelectedSkinIndex);
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyPlayerState, SelectedSkinIndex);
}


void AMyPlayerState::ServerSetSkin_Implementation(int32 NewSkinIndex)
{
    SelectedSkinIndex = NewSkinIndex;
    OnRep_SelectedSkinIndex(); // manually call on server since RepNotify only fires on clients
}

void AMyPlayerState::OnRep_SelectedSkinIndex()
{
    APawn* OwningPawn = GetPawn();
    if (!OwningPawn) return;

    if (ALobbyCharacter* LobbyChar = Cast<ALobbyCharacter>(OwningPawn))
    {
        LobbyChar->ApplySkin(SelectedSkinIndex);
    }
}


void AMyPlayerState::CopyProperties(APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);
    UE_LOG(LogTemp, Warning, TEXT("CopyProperties called - SkinIndex: %d"), SelectedSkinIndex);

    if (AMyPlayerState* PS = Cast<AMyPlayerState>(PlayerState))
    {
        PS->SelectedSkinIndex = SelectedSkinIndex;
    }
}

void AMyPlayerState::OverrideWith(APlayerState* PlayerState)
{
    Super::OverrideWith(PlayerState);
    UE_LOG(LogTemp, Warning, TEXT("OverrideWith called"));

    if (AMyPlayerState* PS = Cast<AMyPlayerState>(PlayerState))
    {
        SelectedSkinIndex = PS->SelectedSkinIndex;
        UE_LOG(LogTemp, Warning, TEXT("OverrideWith - SkinIndex: %d"), SelectedSkinIndex);
    }
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Lobby_MainMenu/LobbyCharacter.h"
#include "EngineUtils.h"
#include "MyGameInstance.h"


void AMyPlayerState::BeginPlay()
{
    Super::BeginPlay();
}

void AMyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AMyPlayerState, SelectedSkinIndex);
}


void AMyPlayerState::ServerSetSkin_Implementation(int32 NewSkinIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("ServerSetSkin called - NewSkinIndex: %d"), NewSkinIndex);
    SelectedSkinIndex = NewSkinIndex;
    OnRep_SelectedSkinIndex();

    // Tell ALL clients to save this skin to their GameInstance
    MulticastSaveSkin(GetPlayerName(), NewSkinIndex);
    UE_LOG(LogTemp, Warning, TEXT("ServerSetSkin - about to multicast for player: '%s', skin: %d"), *GetPlayerName(), NewSkinIndex);
}

void AMyPlayerState::MulticastSaveSkin_Implementation(const FString& PlayerName, int32 SkinIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("MulticastSaveSkin fired - PlayerName: '%s', SkinIndex: %d"), *PlayerName, SkinIndex);

    if (UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>())
    {
        UE_LOG(LogTemp, Warning, TEXT("Multicast saving skin %d for player: %s"), SkinIndex, *PlayerName);
        GI->SavePlayerSkin(PlayerName, SkinIndex);
    }
}

void AMyPlayerState::OnRep_SelectedSkinIndex()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_SelectedSkinIndex fired - SkinIndex: %d"), SelectedSkinIndex);

    for (TActorIterator<ALobbyCharacter> It(GetWorld()); It; ++It)
    {
        ALobbyCharacter* LobbyChar = *It;
        UE_LOG(LogTemp, Warning, TEXT("Found LobbyCharacter - has playerstate: %s"), LobbyChar->GetPlayerState() ? TEXT("YES") : TEXT("NO"));

        if (LobbyChar->GetPlayerState() == this)
        {
            UE_LOG(LogTemp, Warning, TEXT("Matched! Applying skin"));
            LobbyChar->ApplySkin(SelectedSkinIndex);
            return;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No matching LobbyCharacter found"));
}


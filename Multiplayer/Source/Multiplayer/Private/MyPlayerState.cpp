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
    SelectedSkinIndex = NewSkinIndex;
    OnRep_SelectedSkinIndex();

    // Tell ALL clients to save this skin to their GameInstance
    MulticastSaveSkin(GetPlayerName(), NewSkinIndex);
}

void AMyPlayerState::MulticastSaveSkin_Implementation(const FString& PlayerName, int32 SkinIndex)
{

    if (UMyGameInstance* GI = GetWorld()->GetGameInstance<UMyGameInstance>())
    {
        GI->SavePlayerSkin(PlayerName, SkinIndex);
    }
}

void AMyPlayerState::OnRep_SelectedSkinIndex()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_SelectedSkinIndex fired - SkinIndex: %d"), SelectedSkinIndex);

    for (TActorIterator<ALobbyCharacter> It(GetWorld()); It; ++It)
    {
        ALobbyCharacter* LobbyChar = *It;

        if (LobbyChar && LobbyChar->GetPlayerState() == this)
        {
            UE_LOG(LogTemp, Warning, TEXT("Found matching LobbyCharacter - applying skin"));
            LobbyChar->ApplySkin(SelectedSkinIndex);
            return;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("OnRep_SelectedSkinIndex - NO matching LobbyCharacter found"));
}


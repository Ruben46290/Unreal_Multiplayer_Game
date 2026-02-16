// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby_MainMenu/LobbyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Lobby_MainMenu/LobbyGameMode.h"

// Sets default values
ALobbyCharacter::ALobbyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Only Call On The Local Machine
	if (IsLocallyControlled())
	{
		// Is This Machine The Server / Player 1
		if (HasAuthority())
		{
			// This Is The Server -> Play The Animation
			PlayJoiningAnimation();
		}
		else
		{
			// This Is A Client / Player 2 -> Play The Animation On The Server
			Server_PlayJoiningAnimation();
		}
	}
}


void ALobbyCharacter::Server_PlayJoiningAnimation_Implementation()
{
	// Server RPC - Client Calls This To Tell The Server To Play Animation
	Multicast_PlayJoiningAnimation();
}

void ALobbyCharacter::Multicast_PlayJoiningAnimation_Implementation()
{
	// Multicast RPC - Server Calls This To Play Animation On All Clients
	PlayJoiningAnimation();
}


void ALobbyCharacter::Server_SetReady_Implementation(bool bReady)
{
	// Set Saved Ready bool To Input - Calls OnRep
	bIsReady = bReady;

	// Call Visual Feedback Blueprint Event
	OnReadyStatusChanged(bIsReady);

	// Notify The Gamemode
	ALobbyGameMode* GameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnPlayerReadyChanged();
	}

}


void ALobbyCharacter::OnRep_IsReady()
{
	// Call Visual Feedback Blueprint Event
	OnReadyStatusChanged(bIsReady);
}






// Called to bind functionality to input
void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



void ALobbyCharacter::SetCharacterMesh(USkeletalMesh* NewMesh)
{
	Mesh->SetSkeletalMesh(NewMesh);
}



void ALobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyCharacter, bIsReady);
}
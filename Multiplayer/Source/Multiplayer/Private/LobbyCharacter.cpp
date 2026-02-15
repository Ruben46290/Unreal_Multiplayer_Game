// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyCharacter.h"
#include "Net/UnrealNetwork.h"

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

// Add a server RPC to request the animation
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

// Called to bind functionality to input
void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALobbyCharacter::SetCharacterMesh(USkeletalMesh* NewMesh)
{
	Mesh->SetSkeletalMesh(NewMesh);
}


void ALobbyCharacter::SetReady(bool bReady)
{
	bIsReady = bReady;
}

void ALobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyCharacter, bIsReady);
}
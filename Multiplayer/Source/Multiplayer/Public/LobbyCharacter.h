// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LobbyCharacter.generated.h"

UCLASS()
class MULTIPLAYER_API ALobbyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALobbyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Play Joining Animation - Made In Blueprints
	UFUNCTION(BlueprintImplementableEvent)
	void PlayJoiningAnimation();

	// Play Joining Animation On The Server - Calls The Blueprint Function
	UFUNCTION(Server, Reliable)
	void Server_PlayJoiningAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayJoiningAnimation();

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Change Players Mesh
	UFUNCTION(BlueprintCallable, Category = "Customization")
	void SetCharacterMesh(USkeletalMesh* NewMesh);


protected:

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
	bool bIsReady;

	// Function to set ready status
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void SetReady(bool bReady);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "OrderStation.generated.h"


UCLASS()
class MULTIPLAYER_API AOrderStation : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	AOrderStation();

protected:
	virtual void BeginPlay() override;
	// * * * * * * * * * * Components * * * * * * * * * * 
public:

	// Markers For Where NPC'S Can Walk
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Queue")
	TArray<USceneComponent*> QueuePositions;


	// * * * * * * * * * * Make Order * * * * * * * * * * 


};

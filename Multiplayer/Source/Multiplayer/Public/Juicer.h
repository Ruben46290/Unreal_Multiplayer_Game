// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "Juicer.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AJuicer : public AInteractableActor
{
	GENERATED_BODY()

	
public:

	AJuicer();

	// Implement & Override The Interface Function
	virtual void OnInteract_Implementation(AActor* Interactor);

	// Set highlight with color
	virtual void SetHighlight(bool bEnabled, bool bIsClosest = false) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* HandleMeshComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName ItemNeeded = "Bucket";



};

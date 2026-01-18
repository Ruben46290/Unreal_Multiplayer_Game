// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractInterface.h" // Include Interact Interface
#include "Components/SphereComponent.h"
#include "../MultiplayerCharacter.h" // Include Player Character For Casting
#include "GameFramework/Character.h" // Inldude So It Can Cast To Check If Overlapped Actor Is A Character
#include "InteractableActor.generated.h"

UCLASS()
class MULTIPLAYER_API AInteractableActor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor();

	// Implement the interface function
	virtual void OnInteract_Implementation(AActor* Interactor);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// * * * Components * * *
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	//UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCollision;

	// * * * Sphere Overlap Functions * * *
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	ACharacter* OverlappingPlayer;

	// White Highlight Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UMaterialInterface* WhiteHighlightMaterial;

	// Green Highlight Material
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	UMaterialInterface* GreenHighlightMaterial;

public:	
	// Set highlight with color
	UFUNCTION()
	void SetHighlight(bool bEnabled, bool bIsClosest = false);
};

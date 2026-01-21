// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ItemData.h"
#include "MultiplayerCharacter.generated.h"

class AItem;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMultiplayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	


	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	//** Interaction Key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Interact;

	//** Drop Key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Drop;

	//** Throw Key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Throw;


public:
	AMultiplayerCharacter();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	


	// * * * * * * * * * * Interatable Objects Highlights * * * * * * * * * *
protected:
	// Track Nearby Interactable Objects
	UPROPERTY()
	TArray<AInteractableActor*> NearbyInteractables;

	// Track The Closest Interactable
	UPROPERTY()
	AInteractableActor* ClosestInteractable;

	// Update What Interactable Is Closest, Called On Every Interatable Overlap Change
	void UpdateClosestInteractable();

	// Timer For Updating Highlights
	FTimerHandle HighlightUpdateTimer;

public:
	// Add To NearbyInteractables Array, Called By Interactable Actor On Begin Overlap
	UFUNCTION()
	void AddNearbyInteractable(AInteractableActor* Interactable);

	// Remove To NearbyInteractables Array, Called By Interactable Actor On End Overlap
	UFUNCTION()
	void RemoveNearbyInteractable(AInteractableActor* Interactable);


// * * * * * * * * * * Interaction * * * * * * * * * * 
protected:
	// Input Event For Interact
	void OnInteractPressed();

	// Sends Signal To The Server
	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* ActorToInteract);

	// Interaction Sphere Radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 140.0f;

	// * * * * * * * * * * Held Item / Dropping * * * * * * * * * *  
protected:

	// Item Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMeshComponent;

	// Item Drop Location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* ItemDropLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AItem> ItemBlueprintClass;

	// Replicate this variable and call OnRep_HeldItem when it changes
	UPROPERTY(ReplicatedUsing = OnRep_HeldItem)
	FItemData HeldItem;

	UFUNCTION()
	void OnRep_HeldItem();

	// Server RPC for pickup
	UFUNCTION(Server, Reliable)
	void Server_PickupItem(FItemData Item);

	// Server RPC for drop
	UFUNCTION(Server, Reliable)
	void Server_DropCurrentItem();

	void DropCurrentItem();

	UFUNCTION(Server, Reliable)
	void Server_SpawnItem(FVector Location, FName ItemID);

	// Add to GetLifetimeReplicatedProps
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:

	FName GetHeldItemName();

	UFUNCTION(Server, Reliable)
	void ClearHeldItem();

// * * * * * * * * * * Item Throwing * * * * * * * * * *  
protected:

	// * * * * * Functions * * * * *

	// Throwing Input Started
	void StartThrowingHeldItem();

	// Throwing Input Finished
	void StopThrowingHeldItem();

	UFUNCTION(Server, Reliable)
	void Server_ThrowItem(FVector ThrowDirection, float ThrowStrength);

	// * * * * * Variables * * * * *
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
	float MaxThrowCharge = 1.0f;

	// How Fast The Throw Charges
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
	float ChargeRate = 0.5f;

	// Minumum Throwing Force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
	float MinThrowForce = 250.0f;

	// Maximum Throwing Force
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Throwing")
	float MaxThrowForce = 1500.0f;


	UPROPERTY(BlueprintReadOnly, Category = "Throwing")
	float CurrentThrowCharge = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Throwing")
	bool bIsChargingThrow = false;


	// * * * * * * * * * * Throwing Trajectory Visualization * * * * * * * * * *
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Throwing")
	class USplineComponent* TrajectorySpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Throwing")
	int32 TrajectorySteps = 20; // Number of points to simulate

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Throwing")
	float TrajectoryTimeStep = 0.05f; // Time between each simulation step

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Throwing")
	bool bShowLandingMarker = true;

	void UpdateTrajectoryVisualization();

	UPROPERTY(EditAnywhere, Category = "Item|Throwing")
	UStaticMesh* TrajectoryMesh;

	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshComponents;

	void UpdateSplineMeshes();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Throwing")
	UMaterialInterface* TrajectoryMaterial;



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	// Called From An Item When It's Picked Up
	// Public So It Can Be Called From Item Class
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void PickupItem(FItemData Item);
};


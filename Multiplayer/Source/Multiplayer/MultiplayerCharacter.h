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

public:
	AMultiplayerCharacter();


private:
	// Track current material state
	bool bIsUsingBlueMaterial = true;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	


	// * * * Interatable Objects Highlights * * * 

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

protected:
	// * * * Interaction * * * 
	
	// Input Event For Interact
	void OnInteractPressed();

	// Sends Signal To The Server
	UFUNCTION(Server, Reliable)
	void Server_Interact();

	// Find Nearby Interactable Actor, Using Sphere Trace
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* FindInteractableActor();

	// Interaction Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 140.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bDebugInteraction = true;



	// * * * Held Item / Throwing * * * 

	// Item Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMeshComponent;

	// Scene Actor For Where The Item Should Be Dropped When Swapping 2 Items
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* ItemDropLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FItemData HeldItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	TSubclassOf<AItem> ItemBlueprintClass;

	void DropCurrentItem();
	// Spawn An Item On The Server
	// Used Dropping Items When Swapping Items
	UFUNCTION(Server, Reliable)
	void Server_SpawnItem(FVector Location, FName ItemID);


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

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


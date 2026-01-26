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
	virtual void SetHighlight(bool bEnabled, bool bIsClosest = false, AActor* Player = nullptr) override;

protected:

	// * * * * * * * * * * Components * * * * * * * * * * 
	// Handle Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* HandleMeshComponent;

	// Handle Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BucketMeshComponent;

	// * * * * * * * * * * Variables * * * * * * * * * * 

	// Refrence To Item Data Table (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juicer")
	UDataTable* ItemDataTable;

	// Refrence To Item Blueprint Class To Spawn (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Juicer")
	TSubclassOf<AItem> ItemBlueprintClass;

	// Is There Currently A Bucket? (Replicated With OnRep_HasBucket)
	UPROPERTY(ReplicatedUsing = OnRep_HasBucket, BlueprintReadOnly, Category = "Juicer")
	bool bHasBucket = false;

	// Is The Jucing Animation Currently Playing? (Replicated With OnRep_HIsJuicingt)
	UPROPERTY(ReplicatedUsing = OnRep_IsJuicing, BlueprintReadOnly, Category = "Juicer")
	bool bIsJuicing = false;

	bool bHasJuice = false;

	// Store Item Data
	FItemData StoredItemData;

	// * * * * * * * * * * Functions * * * * * * * * * * 
	
	// Replication Event For Bucket Being Changed
	UFUNCTION()
	void OnRep_HasBucket();

	// Replication Event For Juicer Activating / Deactivating
	UFUNCTION()
	void OnRep_IsJuicing();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// Multicast RPC to play animations on all clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnJuiceStart(FLinearColor JuiceColor);

	// Juicable Object Placed -> Start Juicing Animation
	// Implemented In Blueprints
	UFUNCTION(BlueprintImplementableEvent, Category = "Juicer")
	void OnJuiceStart(FLinearColor JuiceColor);

	// Jucing Animation Is Done
	// Called From On JuiceStart();
	UFUNCTION(BlueprintCallable, Category = "Juicer")
	void OnJuiceComplete();
};

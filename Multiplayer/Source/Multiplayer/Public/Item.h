// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "ItemData.h"
#include "Item.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API AItem : public AInteractableActor
{
	GENERATED_BODY()

public:

	// Implement & Override The Interface Function
	virtual void OnInteract_Implementation(AActor* Interactor);


	// Matches With A DataTable Row
	// ReplicatedUsing Makes The Variable Replicatable For Clients
	// When The Value Is Changed OnRep_ItemName Function Is Called On All Clients To Match With Server
	UPROPERTY(ReplicatedUsing = OnRep_ItemName, EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemName;

	// Replication Event For ItemName
	UFUNCTION()
	void OnRep_ItemName();

	// Load Item Data From The DataTable
	bool LoadItemData();

	// Apply Color To Juice
	// Called From Juicer When Spawning In A New Juice
	UFUNCTION(BlueprintImplementableEvent, Category = "Juice")
	void ApplyJuiceColor(FLinearColor JuiceColor);

protected:

	AItem();

	virtual void BeginPlay() override;

	// Reference To The DataTable (Needs To Be Assigned In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UDataTable* ItemDataTable;

public:
	// In Public For Debugging Only
	// Variable To Store Item Data Loaded From The Data Table
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	FItemData CachedItemData;
};

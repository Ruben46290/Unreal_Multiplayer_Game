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

protected:

	virtual void BeginPlay() override;

	// Load Item Data From The DataTable
	bool LoadItemData();

	// Matches With A DataTable Row
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemName;

	// Reference To The DataTable (Needs To Be Assigned In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UDataTable* ItemDataTable;

	// Variable To Store Item Data Loaded From The Data Table
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	FItemData CachedItemData;
};

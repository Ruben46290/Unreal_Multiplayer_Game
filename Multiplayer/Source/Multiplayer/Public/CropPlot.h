// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include <CropData.h>
#include "CropPlot.generated.h"


UCLASS()
class MULTIPLAYER_API ACropPlot : public AInteractableActor
{
	GENERATED_BODY()

	// * * * * * * * * * * Components * * * * * * * * * * 
protected:

	// Crop Mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CropMeshComponent;

	// * * * * * * * * * * Variables * * * * * * * * * * 
	
	// Keep Track Of Planted Crop
	FName CropName;
	bool bHasSeed = false;

	// Refrence To Item Data Table (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crop")
	UDataTable* ItemDataTable;

	// Refrence To Crop Data Table (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crop")
	UDataTable* CropDataTable;

	FCropData* StoredCropData;

	// Refrence To Item Blueprint Class To Spawn (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crop")
	TSubclassOf<AItem> ItemBlueprintClass;

	// Stored Mesh Refrence
	UPROPERTY(ReplicatedUsing = OnRep_CurrentCropMesh, BlueprintReadOnly, Category = "Crop")
	UStaticMesh* CurrentCropMesh = nullptr;

	// Replication Event For Current Crop Mesh
	UFUNCTION()
	void OnRep_CurrentCropMesh();

	// * * * * * * * * * * Functions * * * * * * * * * * 

public:

	ACropPlot();

	// Plant A Crop
	UFUNCTION(BlueprintCallable, Category = "Crop")
	void PlantCrop(FName CropID);

	// Harvest The Crop
	UFUNCTION(BlueprintCallable, Category = "Crop")
	void HarvestCrop(AActor* Harvester);

	// Implement & Override The Interface Function
	virtual void OnInteract_Implementation(AActor* Interactor);
};

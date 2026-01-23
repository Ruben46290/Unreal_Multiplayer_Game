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

	// Item Drop Location
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* ItemDropLocation;
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


	// Timer For Growing
	FTimerHandle GrowingTimer;

	// How Many Seconds Has The Plant Been Growing
	float GrowingProgress;

	// What Stage Is The Crop ( 1,2,3 )
	int GrowingStage;

	// How Long Before Growing Between Phases, Total Growing Time = StageGrowingTime * 3
	float StageGrowingTime = 3.0f;

	// What Is The Next Second Milestone To Go To Next Growth Phase
	float NextGrowingGoal;


	bool bCanBeHarvested = false;

	// * * * * * * * * * * Functions * * * * * * * * * * 

public:

	ACropPlot();

	// Plant A Crop
	UFUNCTION(BlueprintCallable, Category = "Crop")
	void PlantCrop(FName CropID);

	void GrowingTick();

	// Harvest The Crop
	UFUNCTION(BlueprintCallable, Category = "Crop")
	void HarvestCrop();

	UFUNCTION(Server, Reliable)
	void Server_SpawnItem(FName ItemID, int32 Index);

	// Implement & Override The Interface Function
	virtual void OnInteract_Implementation(AActor* Interactor);

	// Replication Event For Current Crop Mesh
	UFUNCTION()
	void OnRep_CurrentCropMesh();
};

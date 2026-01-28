// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include <CropData.h>
#include "Components/WidgetComponent.h"
#include "Widgets/CropPlotWidget.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* IconWidgetComponent;

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

	// Refrence To CropUI
	// Set On BeginPlay, From Reading The Class Set In IconWidgetComponent
	UCropPlotWidget* CropUI;

	// Timer For Growing
	FTimerHandle GrowingTimer;

	// Water Timer
	FTimerHandle WaterTimer;

	// How Many Seconds Has The Plant Been Growing
	float GrowingProgress;

	// What Stage Is The Crop ( 1,2,3 )
	int GrowingStage;

	// How Long Before Growing Between Phases, Total Growing Time = StageGrowingTime * 3
	float StageGrowingTime = 1.0f;

	// What Is The Next Second Milestone To Go To Next Growth Phase
	float NextGrowingGoal;

	// Is The Crop Ready To Be Harvested
	bool bCanBeHarvested = false;

	// * * * * * Water * * * * *

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	float MaxWaterLevel = 10.0f;

	// How Much Water Is Currently In The Crop
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	float CurrentWaterLevel;

	// How Fast Does Water Get Used
	// Editable To Be Diffrent For Diffrent Level Diffilcuties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	float WaterDecayRate = 0.1f;

	// Growing Rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Water")
	float NoWaterPenalty = 4.0f;

	// * * * * * Auto Regrow * * * * *

	// Should The Crop Automatically Regrow After Being Harvested
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	bool bAutoRegrowEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FName AutoRegrowPlantName;


	// * * * * * * * * * * Functions * * * * * * * * * * 

public:

	ACropPlot();

	virtual void BeginPlay() override;

	// Plant A Crop
	UFUNCTION(BlueprintCallable, Category = "Crop")
	void PlantCrop(FName CropID);

	// Timer Tick That Calls Growing Tick & Water Tick
	void TimerTick();

	void GrowingTick();

	void WaterTick();

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

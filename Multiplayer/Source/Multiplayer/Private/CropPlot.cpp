// Fill out your copyright notice in the Description page of Project Settings.


#include "CropPlot.h"
#include "Net/UnrealNetwork.h"
#include "Item.h"

ACropPlot::ACropPlot()
{
	CropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CropMesh"));
	CropMeshComponent->SetupAttachment(MeshComponent);

	// Item Drop Location
	ItemDropLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropLocation"));
	ItemDropLocation->SetupAttachment(MeshComponent);
}


void ACropPlot::OnInteract_Implementation(AActor* Interactor)
{
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Interact")); }

	// If The Crop Is Fully Grown & Can Be Harvested
	if (bCanBeHarvested) {
		HarvestCrop();
		return;
	}

	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	FName PlayerItem = Character->GetHeldItemName();

	if (PlayerItem == "WateringCan") {

		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Use Watering Can")); }
		return;
	}

	// Return If A Seed Is Already Growing
	if (bHasSeed) { return; }

	// Get DataTable Row, (Text Is For Debugging)
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(PlayerItem, TEXT("Crop Interaction"));

	// If The Players Item Can Be Planted
	if (ItemData && ItemData->bCanBePlanted) {

		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Plant Item")); }


		// Plant Crop
		PlantCrop(PlayerItem);

		// Clear Players Item
		Character->ClearHeldItem();
	}
}


void ACropPlot::PlantCrop(FName CropID)
{

	// Store Crop Data
	StoredCropData = CropDataTable->FindRow<FCropData>(CropID, TEXT("Crop Interaction"));

	// Store Current Crop Mesh
	// Used For Replicating The Mesh
	CurrentCropMesh = StoredCropData->Stage1Mesh;

	// Set Crop Mesh To Stage 1
	CropMeshComponent->SetStaticMesh(StoredCropData->Stage1Mesh);

	// Set Has Seed To True
	bHasSeed = true;

	// Set Growing Variables
	GrowingProgress = 0;
	GrowingStage = 1;
	NextGrowingGoal = StageGrowingTime;
	bCanBeHarvested = false;

	// Start Growing Timer
	GetWorldTimerManager().SetTimer(GrowingTimer, this,
		&ACropPlot::GrowingTick, 0.1f, true);

}

void ACropPlot::GrowingTick()
{

	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Green, TEXT("Crop Growth Tick")); }

	// Add 0.1 To Current Progress
	// Function Is Called On A 0.1 Second Timer So 1 Growing Progress = 1 Second
	GrowingProgress += 0.1f;

	// If Growing Progress Hit The Next Growing Goal
	if (GrowingProgress >= NextGrowingGoal) {

		// If The Crop Is Growing From Stage 1
		if (GrowingStage == 1) {

			// Go To Next Stage
			GrowingStage = 2;

			// Update Next Growing Time
			NextGrowingGoal = StageGrowingTime * 2;

			// Set Crop Mesh To Stage 2
			CropMeshComponent->SetStaticMesh(StoredCropData->Stage2Mesh);

			// Used For Replicating The Mesh
			CurrentCropMesh = StoredCropData->Stage2Mesh;

			//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Grow To Stage 2")); }
		}

		// If Crop Is Growing From Stage 2
		else if (GrowingStage == 2) {

			// Set Crop Mesh To Stage 3
			CropMeshComponent->SetStaticMesh(StoredCropData->Stage3Mesh);

			// Used For Replicating The Mesh
			CurrentCropMesh = StoredCropData->Stage3Mesh;

			// Stop Timer
			GetWorldTimerManager().ClearTimer(GrowingTimer);

			// Set Can Be Harvested To True
			bCanBeHarvested = true;

			//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Grow To Stage 3")); }
		}

	}
}

void ACropPlot::HarvestCrop()
{
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Harvest Crop")); }


	// Loop Through All Items To Drop
	for (int32 i = 0; i < StoredCropData->DroppedItems.Num(); i++) {

		// Spawn The Item On The Server
		Server_SpawnItem(StoredCropData->DroppedItems[i], i);
	}

	// Clear Mesh
	CropMeshComponent->SetStaticMesh(nullptr);
	CurrentCropMesh = nullptr;

	bCanBeHarvested = false;
	bHasSeed = false;
}


void ACropPlot::Server_SpawnItem_Implementation(FName ItemID, int32 Index)
{

	FVector SpawnPos = ItemDropLocation->GetComponentLocation();

	AItem* NewItem = GetWorld()->SpawnActorDeferred<AItem>(ItemBlueprintClass, FTransform(FRotator::ZeroRotator, FVector(SpawnPos.X, SpawnPos.Y, SpawnPos.Z + (Index * 50))));

	if (NewItem) {
		
		NewItem->ItemName = ItemID;
		NewItem->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnPos));
	}
}

// * * * * * * * * * * Replication * * * * * * * * * * 

void ACropPlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACropPlot, CurrentCropMesh);
}

// Replication Event For Current Crop Mesh
void ACropPlot::OnRep_CurrentCropMesh()
{
	if (CurrentCropMesh)
	{
		CropMeshComponent->SetStaticMesh(CurrentCropMesh);
	}
}
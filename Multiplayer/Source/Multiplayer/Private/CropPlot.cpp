// Fill out your copyright notice in the Description page of Project Settings.


#include "CropPlot.h"
#include "Net/UnrealNetwork.h"

ACropPlot::ACropPlot()
{
	CropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CropMesh"));
	CropMeshComponent->SetupAttachment(MeshComponent);
}


void ACropPlot::OnInteract_Implementation(AActor* Interactor)
{
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Interact")); }

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

	// Store Current Crop Mesh, That Is Replicated 
	CurrentCropMesh = StoredCropData->Stage1Mesh;

	// Set Crop Mesh To Stage 1
	CropMeshComponent->SetStaticMesh(StoredCropData->Stage1Mesh);

}

void ACropPlot::HarvestCrop(AActor* Harvester)
{
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
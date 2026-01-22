// Fill out your copyright notice in the Description page of Project Settings.


#include "Juicer.h"
#include "Net/UnrealNetwork.h"
#include "Item.h"

AJuicer::AJuicer()
{

	bReplicates = true;

	// Create Handle Mesh And Attach To Main Mesh
	HandleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandleMesh"));
	HandleMeshComponent->SetupAttachment(MeshComponent);

	BucketMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BucketMesh"));
	BucketMeshComponent->SetupAttachment(MeshComponent);
	BucketMeshComponent->SetVisibility(false);

	// bHasBucket Is Replicated Not The Mesh
	BucketMeshComponent->SetIsReplicated(false);
}

void AJuicer::SetHighlight(bool bEnabled, bool bIsClosest)
{
	// If Mesh Is Valid
	if (MeshComponent)
	{
		// Is Highlight Enabled
		if (bEnabled)
		{
			// Green For Closest, White For Others
			MeshComponent->SetOverlayMaterial(bIsClosest ? GreenHighlightMaterial : WhiteHighlightMaterial);
			HandleMeshComponent->SetOverlayMaterial(bIsClosest ? GreenHighlightMaterial : WhiteHighlightMaterial);
		}
		else
		{
			MeshComponent->SetOverlayMaterial(nullptr);
			HandleMeshComponent->SetOverlayMaterial(nullptr);
		}
	}
}


// * * * * * * * * * * Interaction & Juicing * * * * * * * * * * 

void AJuicer::OnInteract_Implementation(AActor* Interactor)
{
	
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Interact Pressed")); }

	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	FName PlayerItem = Character->GetHeldItemName();

	// Does The Player Have A Held Item && Isn't Currently Playing Jucing Animation
	if (PlayerItem != NAME_None && !bIsJuicing) {

		// If There Isn't A Bucket Placed Already
		if (!bHasBucket) {

			// If The Player Is Holding A Bucket
			if (PlayerItem == "Bucket") {

				//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Bucket Placed")); }

				// Clear Held Item
				Character->ClearHeldItem();

				// Show Bucket Mesh
				BucketMeshComponent->SetVisibility(true);

				// Set Has Bucket To True
				bHasBucket = true;

			}
		}
		// Bucket Already Placed
		else{

			// Get DataTable Row, (Text Is For Debugging)
			FItemData* ItemData = ItemDataTable->FindRow<FItemData>(PlayerItem, TEXT("Juicer Interaction"));

			// Store Item Data
			StoredItemData = *ItemData;

			// If Item Data Was Loaded
			if (ItemData) {

				// If Held Item Can Be Juiced
				if (ItemData->bCanBeJuiced) {

					//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Item Placed In Juicer")); }

					// Clear Held Item
					Character->ClearHeldItem();

					// Set Juicing To True
					bIsJuicing = true;

					// Play Animation On All Clients & Server
					Multicast_OnJuiceStart(ItemData->ItemColor);

				}

			}

		}

	}

}


void AJuicer::Multicast_OnJuiceStart_Implementation(FLinearColor JuiceColor)
{
	// This Runs On All Clients & Server
	// Animation Event

	OnJuiceStart(JuiceColor);
}


void AJuicer::OnJuiceComplete()
{
	// Called From Blueprint Animation Event When Animation Is Finished
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("OnJuiceComplete()")); }
	// Reset Variables
	bHasBucket = false;
	bIsJuicing = false;

	// Hide Bucket Mesh
	BucketMeshComponent->SetVisibility(false);

	// if Theres No Valid Data -> Return
	if (StoredItemData.ItemID == NAME_None) {
		return;
	}

	FVector SpawnLocation = BucketMeshComponent->GetComponentLocation();

	// Start Spawning Actor
	AItem* JuiceItem = GetWorld()->SpawnActorDeferred<AItem>(
		ItemBlueprintClass,
		FTransform(FRotator::ZeroRotator, SpawnLocation)
	);

	if (JuiceItem) {

		// Apply Item Name
		JuiceItem->ItemName = FName(*(StoredItemData.ItemID.ToString() + TEXT("Juice")));

		// Finish Spawning
		JuiceItem->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnLocation));

		// Apply Juice Color
		//JuiceItem->ApplyJuiceColor(StoredItemData.ItemColor);
	}

	StoredItemData = FItemData();
}

// * * * * * * * * * * Replication Events * * * * * * * * * * 


// This Runs On Clients When bHasBucket Is Replicated
void AJuicer::OnRep_HasBucket()
{
	if (BucketMeshComponent)
	{
		BucketMeshComponent->SetVisibility(bHasBucket);
	}

}

void AJuicer::OnRep_IsJuicing()
{
}



// Replication Event
void AJuicer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AJuicer, bHasBucket);
	DOREPLIFETIME(AJuicer, bIsJuicing);
}
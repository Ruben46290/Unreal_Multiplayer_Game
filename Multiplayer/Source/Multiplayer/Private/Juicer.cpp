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

void AJuicer::SetHighlight(bool bEnabled, bool bIsClosest, AActor* Player)
{
	// If Mesh Is Valid
	if (MeshComponent)
	{
		// Is Highlight Enabled
		if (bEnabled)
		{
			// If Juicing Animation Is Playing
			if (bIsJuicing) {

				// Clear All Highlights
				MeshComponent->SetOverlayMaterial(nullptr);
				HandleMeshComponent->SetOverlayMaterial(nullptr);
				BucketMeshComponent->SetOverlayMaterial(nullptr);
				return;
			}

			// Is There Juice Waiting To Be Picked Up
			if (bHasJuice) {

				// Cast To Player Class
				AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Player);

				// Get Item Name
				FName PlayerItem = Character->GetHeldItemName();

				// Is The Players Hands Empty
				if (PlayerItem == NAME_None) {

					// Enable Green Highlight
					BucketMeshComponent->SetOverlayMaterial(GreenHighlightMaterial);
				}

				// Player Has Something Else In Their Hands
				else {

					// Enable White Highlight
					BucketMeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);
				}

				// Return To Skip Code Below
				return;
			}

			// If Theres A Bucket Waiting For An Item To Be Placed
			if (bHasBucket) {
				
				// Cast To Player Class
				AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Player);

				// Get Item Name
				FName PlayerItem = Character->GetHeldItemName();

				// If No Item DataTable Has Been Set, Print & Return
				if (GEngine && !ItemDataTable) { GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, TEXT("NO ITEM DATA TABLE SET IN JUICER")); return; }

				// Load From Data Table, Second Paramater Is For Debugging Context
				FItemData* RowData = ItemDataTable->FindRow<FItemData>(PlayerItem, TEXT("JuicerLoadItemData"));

				// If Held Item Can Be Juiced
				if (RowData && RowData->bCanBeJuiced) {

					// Enable Green Highlight
					MeshComponent->SetOverlayMaterial(GreenHighlightMaterial);
					HandleMeshComponent->SetOverlayMaterial(GreenHighlightMaterial);
					BucketMeshComponent->SetOverlayMaterial(GreenHighlightMaterial);

				}

				// No Held Item Or Item Can't Be Juiced
				else {

					// Enable White Highlight
					MeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);
					HandleMeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);
					BucketMeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);
				}
			}

			// If Theres Not A Bucket Placed
			else {

				// Is The Juicer The Closest Interactable
				if (!bIsClosest) {

					// Show White Highlight
					MeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);
					HandleMeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);

					return;
				}

				// Cast To Player Class
				AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Player);

				// Get Item Name
				FName PlayerItem = Character->GetHeldItemName();

				// If The Player Is Holding A Bucket
				if (PlayerItem == "Bucket") {

					// Show Green Highlight
					MeshComponent->SetOverlayMaterial(GreenHighlightMaterial);
					HandleMeshComponent->SetOverlayMaterial(GreenHighlightMaterial);
				}

				// Player Not Holding A Bucket
				else {

					// Show White Highlight
					MeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);
					HandleMeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);
				}


				
			}

		}
		// If Highlight Is Disabled
		else
		{
			MeshComponent->SetOverlayMaterial(nullptr);
			HandleMeshComponent->SetOverlayMaterial(nullptr);
			BucketMeshComponent->SetOverlayMaterial(nullptr);
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
		else {

			// Get DataTable Row, (Text Is For Debugging)
			FItemData* ItemData = ItemDataTable->FindRow<FItemData>(PlayerItem, TEXT("Juicer Interaction"));

			// If Item Data Was Loaded
			if (ItemData) {

				// If Held Item Can Be Juiced
				if (ItemData->bCanBeJuiced) {

					// Store Item Data
					StoredItemData = *ItemData;

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

	// If The Playeres Hands Are Empty & Theres A Juice Waiting
	else if (PlayerItem == NAME_None && bHasJuice) {

		
		// If The Players Hands Are Empty
		if (PlayerItem == NAME_None) {

			// Get Juices Row Name
			FName JuiceName = FName(*(StoredItemData.ItemID.ToString() + TEXT("Juice")));


			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0, FColor::Cyan,
					FString::Printf(TEXT("JuiceName: %s"), *JuiceName.ToString()));
			}


			// Get DataTable Row, (Text Is For Debugging)
			FItemData* JuiceItemData = ItemDataTable->FindRow<FItemData>(JuiceName, TEXT("Juicer Interaction"));

			if (JuiceItemData) {

				// Give Player The Juice
				Character->PickupItem(*JuiceItemData);

				//JuiceItem->ItemName = FName(*(StoredItemData.ItemID.ToString() + TEXT("Juice")));

				// Reset Variables
				bHasBucket = false;
				bHasJuice = false;
				StoredItemData = FItemData();

				// Hide Bucket Mesh
				BucketMeshComponent->SetVisibility(false);
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
	//bHasBucket = false;
	bIsJuicing = false;

	bHasJuice = true;

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
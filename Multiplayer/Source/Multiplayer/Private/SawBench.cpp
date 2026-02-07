// Fill out your copyright notice in the Description page of Project Settings.


#include "SawBench.h"
#include "Net/UnrealNetwork.h"
#include "../MultiplayerCharacter.h"

ASawBench::ASawBench()
{
	// Make Item Mesh
	ItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMeshComponent->SetupAttachment(MeshComponent);
}

void ASawBench::OnInteract_Implementation(AActor* Interactor)
{
	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	// Get Item Name
	FName PlayerItem = Character->GetHeldItemName();

	// If The Player Is Holding A Log & The Bench Is Empty
	if (PlayerItem == "Log" && CurrentState == "Empty") {

		// Remove The Log From The Player
		Character->ClearHeldItem();

		// Apply The Log Mesh
		ItemMeshComponent->SetStaticMesh(LogMesh);

		// Move & Rotate Log Mesh To Fit On Table
		ItemMeshComponent->SetRelativeRotation(FRotator(-90.0f,0.0f,0.0f));
		ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 65.0f));

		// Change The CurrentItemMesh To Trigger OnRep Event
		CurrentItemMesh = LogMesh;

		// Update State, So You Have To Wait For Animation To Finish
		CurrentState = "PlayingAnimation";

		// Play Animation On All Clients & Server
		Multicast_OnAnimationStart();
	}

	// If The Player Has Empty Hands & A Bucket Is Waiting To Be Picked Up
	else if (PlayerItem == NAME_None && CurrentState == "HasBucket") {

		// If No DataTable Was Set In Editor, Print & Return
		if (!ItemDataTable) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("No Data Table Assigned")); return; }

		// Load From Data Table, Second Paramater Is For Debugging Context
		FItemData* RowData = ItemDataTable->FindRow<FItemData>("Bucket", TEXT("LoadItemData"));

		// Send Bucket Item Data To Player To Equip Bucket
		Character->PickupItem(*RowData);

		// Clear Item Mesh
		ItemMeshComponent->SetStaticMesh(nullptr);

		// Replicated Item Mesh As Cleared
		CurrentItemMesh = nullptr;

		// Set State As Empty So Another Log Can Be Placed
		CurrentState = "Empty";

	}
}

void ASawBench::SetHighlight(bool bEnabled, bool bIsClosest, AActor* Player)
{
	// If Mesh Is Valid
	if (MeshComponent)
	{
		// Is Highlight Enabled
		if (bEnabled)
		{
			// If There Is Currently A Bucket Waiting On The Table
			if (ItemMeshComponent && CurrentState == "HasBucket") {

				// If Table Isn't The Closest Interactable
				if (!bIsClosest) {

					// Show Bucket Outline
					ItemMeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);

				}
				// If Table Is The Closest Interactable
				else {

					// Cast To Player Class
					// Casting Is Surpisingly Cheap On Performance So Casting Every 0.1 Seconds Shouldn't Be An Issue
					AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Player);

					// Get Item Name
					FName PlayerItem = Character->GetHeldItemName();

					// If The Player Has Empty Hands Show Green Highlight, If Not Show White Highlight
					ItemMeshComponent->SetOverlayMaterial(PlayerItem == NAME_None ? GreenHighlightMaterial : WhiteHighlightMaterial);


				}

				// If The Body is Highlighted
				if (MeshComponent->GetOverlayMaterial() != nullptr) {

					// Disable The Highlight On The Body
					MeshComponent->SetOverlayMaterial(nullptr);
				}

			}

			// If There Isn't A Bucket On The Table
			else {

				// If The Animation Is Currently Playing
				if (CurrentState == "PlayingAnimation") {

					// Hide Body Highlight
					MeshComponent->SetOverlayMaterial(nullptr);

					// If The Item Is Visible
					if (ItemMeshComponent) {

						// Hide Highlight Around The Item
						ItemMeshComponent->SetOverlayMaterial(nullptr);
					}

					return;

				}

				// If The Table Isn't The Closest Interactable
				if (!bIsClosest) {
					// Set Highlight As White
					MeshComponent->SetOverlayMaterial(WhiteHighlightMaterial);

				}

				// The Table Is The Closest Interactable
				else {

					// Cast To Player Class
					// Casting Is Surpisingly Cheap On Performance So Casting Every 0.1 Seconds Shouldn't Be An Issue
					AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Player);

					// Get Item Name
					FName PlayerItem = Character->GetHeldItemName();

					// If The Player Is Holding A Log Show Green Highlight, If Not Show White Highlight
					MeshComponent->SetOverlayMaterial(PlayerItem == "Log" ? GreenHighlightMaterial : WhiteHighlightMaterial);
				}
			}
		}

		// Highlight Is Not Enabled
		else
		{
			// Hide Body Highlight
			MeshComponent->SetOverlayMaterial(nullptr);

			// If The Item Is Visible
			if (ItemMeshComponent) {

				// Hide Highlight Around The Item
				ItemMeshComponent->SetOverlayMaterial(nullptr);
			}
		}
	}
}


void ASawBench::Multicast_OnAnimationStart_Implementation()
{
	// This Runs On All Clients & Server, Plays Animation Only

	// Function Implemented In Blueprints
	OnAnimationStart();
}






void ASawBench::OnAnimaitionComplete()
{
	// Called From The Overriden Blueprint Animation Event

	// Apply The Bucket Mesh
	ItemMeshComponent->SetStaticMesh(BucketMesh);

	// Move & Rotate Bucket Mesh To Fit On Table
	ItemMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));

	// Change The CurrentItemMesh To Trigger OnRep Event
	CurrentItemMesh = BucketMesh;

	// Update Current State
	CurrentState = "HasBucket";

}



void ASawBench::OnRep_CurrentItemMesh()
{
	// Don't Check If Mesh Is Valid So It Can Update On nullptr Aswell
	ItemMeshComponent->SetStaticMesh(CurrentItemMesh);

	// If There Is A Mesh 
	if (CurrentItemMesh) {

		// If The Logs On The Table
		if (CurrentItemMesh == LogMesh) {

			// Move & Rotate Log Mesh To Fit On Table
			ItemMeshComponent->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
			ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 65.0f));
		}
		
		// Else - Bucket Is Placed On Table
		else {

			// Move & Rotate Bucket Mesh To Fit On Table
			ItemMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
			ItemMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 55.0f));
		}
	}
}


// Replication Event
void ASawBench::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASawBench, CurrentItemMesh);
	DOREPLIFETIME(ASawBench, CurrentState);
}
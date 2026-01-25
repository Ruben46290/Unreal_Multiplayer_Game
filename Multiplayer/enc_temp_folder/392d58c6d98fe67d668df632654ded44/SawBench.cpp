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

		// Load From Data Table, Second Paramater Is For Debugging Context
		FItemData* RowData = ItemDataTable->FindRow<FItemData>("Bucket", TEXT("LoadItemData"));

		// Send Bucket Item Data To Player To Equip Bucket
		Character->PickupItem(*RowData);

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
}


// Replication Event
void ASawBench::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASawBench, CurrentItemMesh);
}
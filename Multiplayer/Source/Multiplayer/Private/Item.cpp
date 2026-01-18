// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Net/UnrealNetwork.h"

void AItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItem, ItemName);
}

AItem::AItem()
{
	bReplicates = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> ItemTableFinder(TEXT("/Game/DataTables/DT_Items"));
	if (ItemTableFinder.Succeeded())
	{
		ItemDataTable = ItemTableFinder.Object;
	}
}


void AItem::BeginPlay()
{
	// Call Parent Class Begin Play (InteractableActor, Bind Sphere Overlap Events)
	Super::BeginPlay();

	// If Item Name Is Valid
	// Item Name Is Set While The Actor Is Being Spawned
	if (ItemName != NAME_None) {
		LoadItemData();
	}

	// Enable physics after spawn
    MeshComponent->SetSimulatePhysics(true);
    MeshComponent->SetEnableGravity(true);
	MeshComponent->SetMassOverrideInKg(NAME_None, 100.0f, true); // Override Mass To 100kg

	bReplicates = true;
}



void AItem::OnRep_ItemName()
{
	// This runs on clients when ItemName replicates
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("On Rep Called")); }
	LoadItemData	();
}

bool AItem::LoadItemData()
{
	// DB If No Data Table Has Been Assigned, Print & Return
	if (!ItemDataTable) {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("No Data Table Assigned")); }
		return false;
	}

	// DB If No Name Is Set, Print & Return
	if (ItemName == NAME_None) {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("No Item Name Assigned")); }
		return false;
	}

	// Load From Data Table, Second Paramater Is For Debugging Context
	FItemData* RowData = ItemDataTable->FindRow<FItemData>(ItemName, TEXT("LoadItemData"));

	// DB If No Data Is Found, Print & Return
	if (!RowData) {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("No Row Data Found")); }
		return false;
	}

	// Store Row Data
	CachedItemData = *RowData;

	// If Item Mesh Is Valid & Item Data Has A Valid Mesh
	if (MeshComponent && CachedItemData.Mesh) {
		
		// Set Mesh To Datatable Mesh
		MeshComponent->SetStaticMesh(CachedItemData.Mesh);
	}

	// Return True
	return true;
}

void AItem::OnInteract_Implementation(AActor* Interactor)
{
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Yellow, TEXT("Item Interact")); }

	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	if (Character) {

		//Character->RemoveNearbyInteractable(this);

		Character->PickupItem(CachedItemData);

		//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Yellow, TEXT("Item Interact")); }
	}

	Destroy();
}
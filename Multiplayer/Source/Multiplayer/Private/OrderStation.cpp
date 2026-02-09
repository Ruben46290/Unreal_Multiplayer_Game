// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderStation.h"
#include "Net/UnrealNetwork.h"

;

AOrderStation::AOrderStation()
{
	bReplicates = true;

	// Setup Icon Widget
	IconWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OrderWidget"));
	IconWidgetComponent->SetupAttachment(MeshComponent);
	IconWidgetComponent->SetVisibility(false);
}

void AOrderStation::BeginPlay()
{
	Super::BeginPlay();

	// Get Order Widget From Widget Component
	OrderWidget = Cast<UOrderWidget>(IconWidgetComponent->GetUserWidgetObject());

	if (!OrderWidget) {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("Order UI NOT LOADED!!!")); return; }
	}

	// CropUI = Cast<UCropPlotWidget>(IconWidgetComponent->GetUserWidgetObject());
}




void AOrderStation::SpawnCustomer(FCustomer CustomerData)
{

	// Only Run On Server
	if (!HasAuthority()) {
		return;
	}

	// If Theres Space In The Queue
	if (CustomersInQueue < QueuePositions.Num()) {


		// Get The Last Points Location & Rotation
		int32 BackPositionIndex = QueuePositions.Num() - 1;
		FVector SpawnLocation = QueuePositions[BackPositionIndex]->GetActorLocation();
		FRotator SpawnRotation = QueuePositions[BackPositionIndex]->GetActorRotation();

		// Set Spawn Paramaters
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// Spawn The Customer
		ACustomerNPC* NewCustomer = GetWorld()->SpawnActor<ACustomerNPC>(
			CustomerBlueprintClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (NewCustomer) {

			//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Spawn Customer")); }

			// Bind Customers Event Dispatcher To OnCustomerReachedStation()
			NewCustomer->OnReachedStation.AddDynamic(this, &AOrderStation::OnCustomerReachedStation);

			// Store The Order On The Customer
			NewCustomer->StoredCustomerData = CustomerData;

			// Add New Customer To Customer Array
			Customers.Add(NewCustomer);
			CustomersInQueue++;

			// Move All Customers Up In Line
			MoveCustomersToPositions();
		}
	}

	// Theres No Space In The Queue
	else {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red, TEXT("No Space In Queue - Skipping Customer")); }
	}


}

void AOrderStation::MoveCustomersToPositions()
{
	// Only Run On Server
	if (!HasAuthority()) {
		return;
	}

	// Loop through all customers and assign them positions
	for (int32 i = 0; i < Customers.Num(); i++)
	{
		// Get Customer
		ACustomerNPC* Customer = Customers[i];
		if (!Customer) continue;

		// Get Customers Current Pos In The Queue
		int32 CurrentPos = FindCustomerPosition(Customer);

		// If Customer Is First In Line Set As True
		Customer->bIsFirstInLine = (i == 0);

		// Where The Customer Needs To Go 
		// Uses Array So Customer[x] Goes To QueuePos[x]
		int32 TargetPos = i;

		// Make An Empty Path
		TArray<FVector> Path;


		if (CurrentPos > TargetPos) {

			// For Each Path Index Below Current Pos
			for (int32 PathIndex = CurrentPos - 1; PathIndex >= TargetPos; PathIndex--)
			{
				if (QueuePositions.IsValidIndex(PathIndex))
				{
					// Add To Path Array
					Path.Add(QueuePositions[PathIndex]->GetActorLocation());
				}
			}
		}

		if (Path.Num() > 0)
		{
			Customer->MoveAlongPath(Path);

		}
	}
}

int32 AOrderStation::FindCustomerPosition(ACustomerNPC* Customer)
{
	if (!Customer) return -1;

	FVector CustomerLocation = Customer->GetActorLocation();
	float ClosestDistance = MAX_FLT;
	int32 ClosestPosition = -1;

	// Find which queue position they're closest to
	for (int32 i = 0; i < QueuePositions.Num(); i++)
	{
		if (!QueuePositions[i]) continue;

		FVector PosLocation = QueuePositions[i]->GetActorLocation();
		float Distance = FVector::Dist2D(CustomerLocation, PosLocation);

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPosition = i;
		}
	}

	return ClosestPosition;
}


void AOrderStation::OnCustomerReachedStation(ACustomerNPC* Customer)
{
	// Only Run On Server
	if (!HasAuthority()) { return; }
	
	// If Customer Isn't Valid -> Return
	if (!Customer) { return; }

	// Load & Store Customers Order
	CurrentCustomerData = Customer->StoredCustomerData;

	// Update UI
	UpdateOrderUI();
}

void AOrderStation::UpdateOrderUI()
{
	// If Widget Is Valid
	if (OrderWidget) {

		// Show UI
		OrderWidget->SetVisibility(ESlateVisibility::Visible);

		// Convert Enums To FNames
		TArray<FName> ItemNames;
		for (EItemType ItemType : CurrentCustomerData.RequiredItems)
		{
			FName ItemName = FName(*UEnum::GetValueAsString(ItemType));
			ItemNames.Add(ItemName);
		}

		// Update UI
		OrderWidget->MakeOrder(ItemNames);

	}
}

void AOrderStation::OnInteract_Implementation(AActor* Interactor)
{
	// Check if there's a customer to serve
	if (Customers.Num() == 0) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Orange,
				TEXT("No customers to serve!"));
		}
		return;
	}

	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	// If Character Is Invalid -> Print & Return
	if (!Character) { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, TEXT("Order Station Interact Player Cast Invalid!!!")); } return; }

	// Get Item Name
	FName PlayerItem = Character->GetHeldItemName();

	// If Players Hands Are Empty
	if (PlayerItem == NAME_None) {
		return;
	}

	// Check if player item matches any required item
	bool bItemFound = false;
	int32 FoundIndex = -1;

	FString PlayerItemString = PlayerItem.ToString();
	for (int32 i = 0; i < CurrentCustomerData.RequiredItems.Num(); i++)
	{
		FString EnumString = UEnum::GetValueAsString(CurrentCustomerData.RequiredItems[i]);
		FString EnumValueName;
		EnumString.Split(TEXT("::"), nullptr, &EnumValueName);

		if (EnumValueName.Equals(PlayerItemString))
		{
			bItemFound = true;
			FoundIndex = i;
			break;
		}
	}

	// If The Player Has An Item That Is In The Order
	if (bItemFound) {
		
		Character->ClearHeldItem();

		// Remove the item by index
		CurrentCustomerData.RequiredItems.RemoveAt(FoundIndex);

		// Remake UI
		// Should Be Cheap To Remake 10 Widgets Max 
		// Convert Enums To FNames
		// Convert Enums To FNames
		TArray<FName> ItemNames;
		for (EItemType ItemType : CurrentCustomerData.RequiredItems)
		{
			FName ItemName = FName(*UEnum::GetValueAsString(ItemType));
			ItemNames.Add(ItemName);
		}
		// Update UI
		OrderWidget->MakeOrder(ItemNames);

		// Clear Players Hands
		//Character->ClearHeldItem();
		// 
		// If The Order Is Now Empty
		if (CurrentCustomerData.RequiredItems.Num() == 0) {
			// Clear The First Customer
			ServeFirstCustomer();
		}
	}
	else {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Wrong Item")); }
	}
}

// Clear The First Customer And Move The Rest Up
void AOrderStation::ServeFirstCustomer()
{
	// Only Run On Server
	if (!HasAuthority()) return;

	if (Customers.Num() == 0) return;

	// Get the first customer
	ACustomerNPC* FirstCustomer = Customers[0];

	// Remove from array
	Customers.RemoveAt(0);
	CustomersInQueue--;

	// Destroy the customer actor
	if (FirstCustomer) {
		FirstCustomer->Destroy();
	}

	// Move remaining customers forward
	MoveCustomersToPositions();

	// If Widget Is Valid
	if (OrderWidget) {

		// Show UI
		//OrderWidget->SetVisibility(ESlateVisibility::Hidden);

	}
}


// * * * * * * * * * * Replication * * * * * * * * * * 

void AOrderStation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOrderStation, CurrentCustomerData);
}

// When CurrentCustomerData Is Updated On Server
void AOrderStation::OnRep_CurrentCustomerData()
{
	// Update Order Widget On Clients
	UpdateOrderUI();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderStation.h"

;

AOrderStation::AOrderStation()
{
}

void AOrderStation::BeginPlay()
{
	Super::BeginPlay();
}




void AOrderStation::SpawnCustomer(FOrder Order)
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

			// Add New Customer To Customer Array
			Customers.Add(NewCustomer);

			CustomersInQueue++;

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
	// If Customer Isn't Valid -> Return
	if (!Customer) { return; }


	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Customer Reached Station")); }

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

	// Serve the first customer
	ServeFirstCustomer();
}

void AOrderStation::ServeFirstCustomer()
{
	// Only Run On Server
	if (!HasAuthority()) return;

	if (Customers.Num() == 0) return;

	// Get the first customer
	ACustomerNPC* FirstCustomer = Customers[0];

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green,
			TEXT("Serving customer!"));
	}

	// Remove from array
	Customers.RemoveAt(0);
	CustomersInQueue--;

	// Destroy the customer actor
	if (FirstCustomer) {
		FirstCustomer->Destroy();
	}

	// Move remaining customers forward
	MoveCustomersToPositions();
}
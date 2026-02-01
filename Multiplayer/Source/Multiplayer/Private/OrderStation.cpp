// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderStation.h"

;

AOrderStation::AOrderStation()
{
}

void AOrderStation::BeginPlay()
{
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

			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Spawn Customer")); }

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
		ACustomerNPC* Customer = Customers[i];
		if (!Customer) continue;


		if (!QueuePositions.IsValidIndex(i)) continue;

		FVector TargetPosition = QueuePositions[i]->GetActorLocation();

		
		Customer->MoveToPosition(TargetPosition);

		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Cyan,
				FString::Printf(TEXT("Customer %d moving to position %d"), i, i));
		}
	}

}

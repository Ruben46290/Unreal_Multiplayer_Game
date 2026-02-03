// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderManager.h"
#include "Net/UnrealNetwork.h"
#include "OrderStation.h"

// Sets default values
AOrderManager::AOrderManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOrderManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Check If Customer Sequence Was Made
	if (CustomerSequence.Num() == 0)
	{
		if (GEngine){GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red,TEXT("No customers in sequence!"));}

		bIsSpawningEnabled = false;
		return;
	}

	// Get Spawn Time Of First Customer
	NextSpawnTime = CustomerSequence[0].SpawnTime;
}

void AOrderManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsSpawningEnabled) { return; }

    // Add Delta Time To Game Time
    GameTime += DeltaTime;

    // If GameTime Is Equal To NextSpawnTime
    if (GameTime >= NextSpawnTime) {

        // Spawn Customer
        SpawnCustomer(CustomerSequence[CurrentSpawnIndex].StationIndex, CustomerSequence[CurrentSpawnIndex].OrderIndex);

        // Increment Spawn Index
        CurrentSpawnIndex += 1;

        // If All The Customers Been Spawned
        if (!CustomerSequence.IsValidIndex(CurrentSpawnIndex)) {

            if (GEngine) {GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green,TEXT("All Customers Spawned")); }

            // Disable Spawning
            bIsSpawningEnabled = false;
            return;
        }

        // Get Next Spawn Time
        NextSpawnTime = CustomerSequence[CurrentSpawnIndex].SpawnTime;
    }
}



void AOrderManager::SpawnCustomer(int32 StationIndex, int32 OrderIndex)
{

	// Get Chosen Station
	AOrderStation* Station = RegisteredStations[StationIndex];

	// If Theres No Station -> Print & Return
	if (!Station) { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, TEXT("SpawnCustomer() Station Invalid")); } return; }

	// Get Chosen Order
	FOrder Order = LevelOrderSequence[OrderIndex];

	// If Theres No Station -> Print & Return
	if (Order.RequiredItems[0] == NAME_None) { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, TEXT("SpawnCustomer() Order Invalid")); } return; }

	// Spawn Customer At The Station
	Station->SpawnCustomer(Order);
}






// * * * * * * * * * * Replication * * * * * * * * * * *


void AOrderManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AOrderManager, CurrentSpawnIndex);

}


//void AOrderManager::OnRep_CurrentOrderIndex()
//{
//}





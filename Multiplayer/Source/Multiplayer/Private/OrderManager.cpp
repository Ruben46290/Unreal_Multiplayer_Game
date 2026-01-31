// Fill out your copyright notice in the Description page of Project Settings.


#include "OrderManager.h"
#include "Net/UnrealNetwork.h"

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
	if (!bIsSpawningEnabled) { return; }

	// Add Delta Time To Game Time
	GameTime += DeltaTime;

	// If GameTime Is Equal To NextSpawnTime
	if (GameTime >= NextSpawnTime) {

		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Spawn Customer")); }

		// Change To Next Spawn Index
		CurrentSpawnIndex += 1;

		// If All Customers Have Been Spawned
		if (CurrentSpawnIndex == CustomerSequence.Num()) {

			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("All Customers Spawned")); }

			// Disable Tick & Return
			bIsSpawningEnabled = false;
			return;
		}

		// Get & Store Next Spawn Time
		NextSpawnTime = CustomerSequence[CurrentSpawnIndex].SpawnTime;

	}

}



bool AOrderManager::RequestNextOrder(AOrderStation* RequestingStation)
{
	return false;
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





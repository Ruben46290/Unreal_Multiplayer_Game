// Fill out your copyright notice in the Description page of Project Settings.


#include "Ordering/OrderManager.h"
#include "Net/UnrealNetwork.h"
#include "Ordering/OrderStation.h"

// Sets default values
AOrderManager::AOrderManager()
{
    bReplicates = true;

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

    // Only Spawn On Server
    if (!HasAuthority()) { return; }

    if (!bIsSpawningEnabled) { return; }

    // Add Delta Time To Game Time
    GameTime += DeltaTime;

    // If GameTime Is Equal To NextSpawnTime
    if (GameTime >= NextSpawnTime) {

        // Spawn Customer
        SpawnCustomer(CustomerSequence[CurrentSpawnIndex]);

        // Increment Spawn Index
        CurrentSpawnIndex += 1;

        // If All The Customers Been Spawned
        if (!CustomerSequence.IsValidIndex(CurrentSpawnIndex)) {

            //if (GEngine) {GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green,TEXT("All Customers Spawned")); }

            // Disable Spawning
            bIsSpawningEnabled = false;
            return;
        }

        // Get Next Spawn Time
        NextSpawnTime = CustomerSequence[CurrentSpawnIndex].SpawnTime;
    }
}



void AOrderManager::SpawnCustomer(FCustomer CustomerData)
{

	// Get Chosen Station
	AOrderStation* Station = RegisteredStations[CustomerData.StationIndex];

	// If Theres No Station -> Print & Return
	if (!Station) { if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 10.0, FColor::Red, TEXT("SpawnCustomer() Station Invalid")); } return; }

    // If Points To Give Hasn't Been Overriden For The Customer
    if (CustomerData.PointsToGive == 0.0f) {

        // Load PointsToGive From CalculateOrderPrice()
        CustomerData.PointsToGive = CalculateOrderPrice(CustomerData.RequiredItems);
    }

	// Spawn Customer At The Station
	Station->SpawnCustomer(CustomerData);
}


// Loop Through All Required Items And Get The Total Point Amount
float AOrderManager::CalculateOrderPrice(TArray<EItemType> RequiredItems)
{
    float TotalPrice = 0;

    if (!ItemDataTable)
    {
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("OrderManager - ItemDataTable is null!")); }
        return 0;
    }

    // For Each Required Item
    for (EItemType ItemType : RequiredItems) {

        // Convert Enum To Clean FName
        FString EnumString = UEnum::GetValueAsString(ItemType);
        FString EnumValueName;
        EnumString.Split(TEXT("::"), nullptr, &EnumValueName);
        FName CleanFName = FName(*EnumValueName);

        // Load From Data Table
        FItemData* ItemData = ItemDataTable->FindRow<FItemData>(CleanFName, TEXT("Calculate Price"));

        // If Item Data Row Is Found
        if (ItemData) {

            // Add Item Price To Total Price
            TotalPrice += ItemData->OrderPrice;
        }

    }

    return TotalPrice;
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





// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkStations/CropPlot.h"
#include "Net/UnrealNetwork.h"
#include "Item.h"


ACropPlot::ACropPlot()
{
	CropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CropMesh"));
	//CropMeshComponent->SetupAttachment(MeshComponent);

	// Item Drop Location
	ItemDropLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropLocation"));
	ItemDropLocation->SetupAttachment(MeshComponent);

	ItemDropLocation2 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropLocation2"));
	ItemDropLocation2->SetupAttachment(MeshComponent);

	ItemDropLocation3 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropLocation3"));
	ItemDropLocation3->SetupAttachment(MeshComponent);

	// Set Up Item Drop Location Array
	ItemDropLocations = { ItemDropLocation, ItemDropLocation2, ItemDropLocation3 };

	IconWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("IconWidget"));
	IconWidgetComponent->SetupAttachment(MeshComponent);
	IconWidgetComponent->SetVisibility(false);
}

void ACropPlot::BeginPlay()
{
	
	Super::BeginPlay();

	// Get The Crop UI
	CropUI = Cast<UCropPlotWidget>(IconWidgetComponent->GetUserWidgetObject());
	
	if (!CropUI) {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("CROP UI NOT LOADED!!!")); return; }
	}

	// Is Auto Regrowing Enabled?
	if (bAutoRegrowEnabled) {

		// Plant Chosen Crop Type
		PlantCrop(AutoRegrowPlantName);
	}
}

void ACropPlot::OnInteract_Implementation(AActor* Interactor)
{
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Interact")); }

	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	// Get Item Name
	FName PlayerItem = Character->GetHeldItemName();

	if (PlayerItem == "WateringCan") {

		// Set Current Water To Max Water
		CurrentWaterLevel = MaxWaterLevel;

		// Update Water UI, 1 = Full Bar
		CropUI->UpdateWaterUI(1);

		// If Timer Isn't Active
		if (!GetWorld()->GetTimerManager().IsTimerActive(GrowingTimer)) {

			// Start Growing Timer
			GetWorldTimerManager().SetTimer(GrowingTimer, this,
				&ACropPlot::TimerTick, 0.1f, true);
		}

		// Stop Function To Skip Other Planting Logic
		return;
	}


	// If The Crop Is Fully Grown & Can Be Harvested
	if (bCanBeHarvested) {
		HarvestCrop();
		return;
	}

	// Get DataTable Row, (Text Is For Debugging)
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(PlayerItem, TEXT("Crop Interaction"));

	// If The Players Item Can Be Planted
	if (ItemData && ItemData->bCanBePlanted) {

		// Return If A Seed Is Already Growing
		if (bHasSeed) {

			// Plyaer Is Placing The Same Seed That Is Planted
			if (PlayerItem == SeedType ) {

				// Add 1 To The Seed Count
				SeedCount += 1;

				// Clear Players Item
				Character->ClearHeldItem();

				// Update Seed Count UI
				CropUI->UpdateSeedCount(SeedCount);
			}

			// Player Is Hold A Diffrent Type Of Seed
			else {
				return;
			}

		}

		// There Isn't A Seed Already Planted
		else {

			// Plant Crop
			PlantCrop(PlayerItem);

			// Clear Players Item
			Character->ClearHeldItem();
		}


	}
}



void ACropPlot::PlantCrop(FName CropID)
{

	// Store Crop Data
	StoredCropData = CropDataTable->FindRow<FCropData>(CropID, TEXT("Crop Plant"));

	// If No Crop Data Was Found
	if (!StoredCropData) {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, TEXT("No Crop Data Found")); }
		return;
	}

	// Store Current Crop Mesh
	// Used For Replicating The Mesh
	CurrentCropMesh = StoredCropData->Stage1Mesh;

	// Set Crop Mesh To Stage 1
	CropMeshComponent->SetStaticMesh(StoredCropData->Stage1Mesh);

	// Set Has Seed To True
	bHasSeed = true;

	// Set Seed Type
	SeedType = CropID;

	// Set Growing Variables
	GrowingProgress = 0;
	GrowingStage = 1;
	NextGrowingGoal = StageGrowingTime;
	bCanBeHarvested = false;

	// If Timer Isn't Active
	if (!GetWorld()->GetTimerManager().IsTimerActive(GrowingTimer)) {

		// Start Growing Timer
		GetWorldTimerManager().SetTimer(GrowingTimer, this,
			&ACropPlot::TimerTick, 0.1f, true);
	}

	// Update Seed Image
	CropUI->UpdateSeedUI(StoredCropData->IconTexture);

	// Update Seed Count UI
	CropUI->UpdateSeedCount(SeedCount);
}

void ACropPlot::TimerTick()
{
	// Set bTickCalled As False
	bool bTickCalled = false;

	// If There Is A Seed That Isn't Done Growing
	if (bHasSeed && !bCanBeHarvested) {

		// Call Growing Tick 
		GrowingTick();
		bTickCalled = true;
	}

	// If There Is Water At The Plot
	if (CurrentWaterLevel > 0) {

		// Call Water Tick
		WaterTick();
		bTickCalled = true;
	}

	// If Neither Of The Ticks Were Called
	if (!bTickCalled) {

		//Stop Timer
		GetWorldTimerManager().ClearTimer(GrowingTimer);
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, TEXT("Stop Growth Tick")); }
	}
}

void ACropPlot::GrowingTick()
{

	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Green, TEXT("Crop Growth Tick")); }

	// Is There Any Water?
	if (CurrentWaterLevel > 0) {

		// Yes - Don't Apply Growing Penalty
		GrowingProgress += GrowingSpeed;
	}
	else {

		// No - Apply Growing Penalty
		GrowingProgress += GrowingSpeed / NoWaterPenalty;
	}

	// If Growing Progress Hit The Next Growing Goal
	if (GrowingProgress >= NextGrowingGoal) {

		// If The Crop Is Growing From Stage 1
		if (GrowingStage == 1) {

			// Go To Next Stage
			GrowingStage = 2;

			// Update Next Growing Time
			NextGrowingGoal = StageGrowingTime * 2;

			// Set Crop Mesh To Stage 2
			CropMeshComponent->SetStaticMesh(StoredCropData->Stage2Mesh);

			// Used For Replicating The Mesh
			CurrentCropMesh = StoredCropData->Stage2Mesh;

			//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Grow To Stage 2")); }
		}

		// If Crop Is Growing From Stage 2
		else if (GrowingStage == 2) {

			// Set Crop Mesh To Stage 3
			CropMeshComponent->SetStaticMesh(StoredCropData->Stage3Mesh);

			// Used For Replicating The Mesh
			CurrentCropMesh = StoredCropData->Stage3Mesh;

			// Stop Timer
			//GetWorldTimerManager().ClearTimer(GrowingTimer);

			// Set Can Be Harvested To True
			bCanBeHarvested = true;

			//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Grow To Stage 3")); }
		}
	}
}

void ACropPlot::WaterTick()
{
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Green, TEXT("Water Tick")); }

	CurrentWaterLevel -= WaterDecayRate;

	CropUI->UpdateWaterUI(CurrentWaterLevel/MaxWaterLevel);
}

void ACropPlot::HarvestCrop()
{

	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Green, TEXT("Crop HarvestCrop()")); }

	// Loop Through All Items To Drop
	for (int32 i = 0; i < StoredCropData->DroppedItems.Num(); i++) {

		// Spawn The Item On The Server
		Server_SpawnItem(StoredCropData->DroppedItems[i], i);
	}

	// Clear Mesh
	CropMeshComponent->SetStaticMesh(nullptr);
	CurrentCropMesh = nullptr;

	// Reset State Variables
	bCanBeHarvested = false;
	//bHasSeed = false;

	// Auto Regrowing Is Enabled
	if (bAutoRegrowEnabled) {

		// Plant Chosen Crop Type
		PlantCrop(AutoRegrowPlantName);
	}

	// Auto Regrow Is Disabled
	else {

		// Is There A Queued Seed To Plant
		if (SeedCount > 0) {

			// Decrease Seed Count By 1
			SeedCount -= 1;

			// Replant Crop
			PlantCrop(SeedType);

		}

		// No More Seeds To Plant
		else {

			// Clear Variables
			bHasSeed = false;
			SeedType = "Null";

			CropUI->UpdateSeedUI(nullptr);

		}

		// Update Seed Count UI
		CropUI->UpdateSeedCount(SeedCount);
	}


}


void ACropPlot::Server_SpawnItem_Implementation(FName ItemID, int32 Index)
{
	// Get Spawn Pos
	FVector SpawnPos = ItemDropLocations[Index]->GetComponentLocation();

	// Start Spawning Item
	AItem* NewItem = GetWorld()->SpawnActorDeferred<AItem>(ItemBlueprintClass, FTransform(FRotator::ZeroRotator, SpawnPos));

	// If An Item Was Spawned
	if (NewItem) {
		
		// Set ItemID
		NewItem->ItemName = ItemID;

		// Finish Spawning Item
		NewItem->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnPos));
	}
}

// * * * * * * * * * * Replication * * * * * * * * * * 

void ACropPlot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACropPlot, CurrentCropMesh);
	DOREPLIFETIME(ACropPlot, CurrentWaterLevel);
	DOREPLIFETIME(ACropPlot, SeedType);
	DOREPLIFETIME(ACropPlot, SeedCount);
}

// Replication Event For Current Crop Mesh
void ACropPlot::OnRep_CurrentCropMesh()
{
	// Don't Check If Mesh Is Valid So It Can Update On nullptr Aswell
	CropMeshComponent->SetStaticMesh(CurrentCropMesh);

}

void ACropPlot::OnRep_CurrentWaterLevel()
{
	// If CropUI Is Valid
	if (CropUI) {

		// Update Water Level
		CropUI->UpdateWaterUI(CurrentWaterLevel / MaxWaterLevel);
	}
}

void ACropPlot::OnRep_SeedType()
{
	if (CropUI && SeedType != NAME_None) {
		FCropData* CropData = CropDataTable->FindRow<FCropData>(SeedType, TEXT(""));
		if (CropData) {
			CropUI->UpdateSeedUI(CropData->IconTexture);
		}
	}
}

void ACropPlot::OnRep_SeedCount()
{
	// Update Seed Count UI
	CropUI->UpdateSeedCount(SeedCount);
}

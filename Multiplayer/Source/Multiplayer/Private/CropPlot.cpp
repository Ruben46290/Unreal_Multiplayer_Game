// Fill out your copyright notice in the Description page of Project Settings.


#include "CropPlot.h"
#include "Net/UnrealNetwork.h"
#include "Item.h"


ACropPlot::ACropPlot()
{
	CropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CropMesh"));
	//CropMeshComponent->SetupAttachment(MeshComponent);

	// Item Drop Location
	ItemDropLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropLocation"));
	ItemDropLocation->SetupAttachment(MeshComponent);

	IconWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("IconWidget"));
	IconWidgetComponent->SetupAttachment(MeshComponent);
	IconWidgetComponent->SetVisibility(false);
}

void ACropPlot::BeginPlay()
{
	
	Super::BeginPlay();

	// Is Auto Regrowing Enabled?
	if (bAutoRegrowEnabled) {

		// Plant Chosen Crop Type
		PlantCrop(AutoRegrowPlantName);
	}

	// Get The Crop UI
	CropUI = Cast<UCropPlotWidget>(IconWidgetComponent->GetUserWidgetObject());
	
	if (!CropUI) {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 100, FColor::Red, TEXT("CROP UI NOT LOADED!!!")); }
	}

}

void ACropPlot::OnInteract_Implementation(AActor* Interactor)
{
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Crop Interact")); }

	// If The Crop Is Fully Grown & Can Be Harvested
	if (bCanBeHarvested) {
		HarvestCrop();
		return;
	}

	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	// Get Item Name
	FName PlayerItem = Character->GetHeldItemName();

	if (PlayerItem == "WateringCan") {

		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Use Watering Can")); }

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

	// Return If A Seed Is Already Growing
	if (bHasSeed) { return; }

	// Get DataTable Row, (Text Is For Debugging)
	FItemData* ItemData = ItemDataTable->FindRow<FItemData>(PlayerItem, TEXT("Crop Interaction"));

	// If The Players Item Can Be Planted
	if (ItemData && ItemData->bCanBePlanted) {

		// Plant Crop
		PlantCrop(PlayerItem);

		// Clear Players Item
		Character->ClearHeldItem();
	}
}


void ACropPlot::PlantCrop(FName CropID)
{

	// Store Crop Data
	StoredCropData = CropDataTable->FindRow<FCropData>(CropID, TEXT("Crop Interaction"));

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
}

void ACropPlot::TimerTick()
{
	bool bTickCalled = false;

	if (bHasSeed && !bCanBeHarvested) {
		GrowingTick();
		bTickCalled = true;
	}

	if (CurrentWaterLevel > 0) {
		WaterTick();
		bTickCalled = true;
	}

	if (!bTickCalled) {
		// If Neither Of The Ticks Called -< Stop Timer
		GetWorldTimerManager().ClearTimer(GrowingTimer);
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Red, TEXT("Stop Growth Tick")); }
	}
}

void ACropPlot::GrowingTick()
{

	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Green, TEXT("Crop Growth Tick")); }

	// Is There Any Water?
	if (CurrentWaterLevel > 0) {

		// Yes - Don't Apply Growing Penalty
		GrowingProgress += 0.1f;
	}
	else {

		// No - Apply Growing Penalty
		GrowingProgress += 0.1f / NoWaterPenalty;
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
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Green, TEXT("Water Tick")); }

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
	bHasSeed = false;

	// Is Auto Regrowing Enabled?
	if (bAutoRegrowEnabled) {

		// Plant Chosen Crop Type
		PlantCrop(AutoRegrowPlantName);
	}
}


void ACropPlot::Server_SpawnItem_Implementation(FName ItemID, int32 Index)
{
	// Get Spawn Pos
	FVector SpawnPos = ItemDropLocation->GetComponentLocation();

	// Spawn Each Item Slightly Higher & Forward
	SpawnPos.X += Index * 75;
	SpawnPos.Z += Index * 50;

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
}

// Replication Event For Current Crop Mesh
void ACropPlot::OnRep_CurrentCropMesh()
{
	// Don't Check If Mesh Is Valid So It Can Update On nullptr Aswell
	CropMeshComponent->SetStaticMesh(CurrentCropMesh);

}
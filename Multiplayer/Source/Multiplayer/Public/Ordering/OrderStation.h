// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "OrderManager.h"
#include "Engine/TargetPoint.h"
#include "CustomerNPC.h"
#include "Components/WidgetComponent.h"
#include "Widgets/OrderWidget.h"
#include "OrderStation.generated.h"

//struct FOrder;

UCLASS()
class MULTIPLAYER_API AOrderStation : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	AOrderStation();

protected:
	virtual void BeginPlay() override;

	// Override Set Highlight Function
	virtual void SetHighlight(bool bEnabled, bool bIsClosest = false, AActor* Player = nullptr) override;

	// Override Overlap Function To Check For Items Being Thrown At The Station
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;

	bool bShowingCustomerUI = false;
public:

	// Markers For Where NPC'S Can Walk
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Queue")
	TArray<ATargetPoint*> QueuePositions;

	// Widget Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* IconWidgetComponent;

	UOrderWidget* OrderWidget;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateOrderUI(float PatiencePercent);

	// * * * * * * * * * * Spawn Customer * * * * * * * * * * 

	void SpawnCustomer(FCustomer CustomerData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer")
	TSubclassOf<ACustomerNPC> CustomerBlueprintClass;
		
	int32 CustomersInQueue = 0;

	UPROPERTY(Replicated)
	TArray<ACustomerNPC*> Customers;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentCustomerData)
	FCustomer CurrentCustomerData;

	void UpdateOrderUI();

	UFUNCTION(BlueprintCallable, Category = "Queue")
	void MoveCustomersToPositions();

	int32 FindCustomerPosition(ACustomerNPC* Customer);

	// Called From Event Dispatcher
	UFUNCTION()
	void OnCustomerReachedStation(ACustomerNPC* Customer);

	// Called From Event Dispatcher
	UFUNCTION()
	void OnCustomerLeaves(ACustomerNPC* Customer);

	// * * * * * * * * * * Interaction * * * * * * * * * * 
	
	// Implement & Override The Interface Function
	virtual void OnInteract_Implementation(AActor* Interactor);


	// Clear The First Customer And Move The Rest Up
	void ServeFirstCustomer();

	// * * * * * * * * * * Replication * * * * * * * * * * 

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnRep_CurrentCustomerData();

	// Store A Refrence To The Patience Boost From The Level Settings So We Can Apply It To Customers When They Spawn - Instead Of Finding The Level Settings Actor Every Time A Customer Spawns
	float StoredCustomerPatienceBoost = 1.0f;
};

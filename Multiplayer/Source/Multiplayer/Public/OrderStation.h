// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "OrderManager.h"
#include "Engine/TargetPoint.h"
#include "CustomerNPC.h"
#include "Components/WidgetComponent.h"
#include "OrderWidget.h"
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
	// * * * * * * * * * * Components * * * * * * * * * * 
public:

	// Markers For Where NPC'S Can Walk
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Queue")
	TArray<ATargetPoint*> QueuePositions;

	// Widget Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* IconWidgetComponent;

	UOrderWidget* OrderWidget;

	// * * * * * * * * * * Spawn Customer * * * * * * * * * * 

	void SpawnCustomer(FCustomer CustomerData);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customer")
	TSubclassOf<ACustomerNPC> CustomerBlueprintClass;
		
	int32 CustomersInQueue = 0;

	TArray<ACustomerNPC*> Customers;

	FCustomer CurrentCustomer;

	UFUNCTION(BlueprintCallable, Category = "Queue")
	void MoveCustomersToPositions();

	int32 FindCustomerPosition(ACustomerNPC* Customer);

	// Called From Event Dispatcher
	UFUNCTION()
	void OnCustomerReachedStation(ACustomerNPC* Customer);



	// * * * * * * * * * * Interaction * * * * * * * * * * 
	
	// Implement & Override The Interface Function
	virtual void OnInteract_Implementation(AActor* Interactor);


	// Clear The First Customer And Move The Rest Up
	void ServeFirstCustomer();

	// * * * * * * * * * * Helper Functions * * * * * * * * * * 

};

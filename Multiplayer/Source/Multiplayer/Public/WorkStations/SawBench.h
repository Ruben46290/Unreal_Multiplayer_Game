// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "WorkStations/WorkStationWidgets/SawBenchWidget.h"
#include "Components/WidgetComponent.h"
#include "SawBench.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYER_API ASawBench : public AInteractableActor
{
	GENERATED_BODY()
	

	// * * * * * * * * * * Components * * * * * * * * * * 
protected:

	// Mesh Component For Log & Bowl
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMeshComponent;

	// Widget Component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* WidgetComponent;

	// * * * * * * * * * * Variables * * * * * * * * * * 


	// Stored Mesh Refrence - Only Used For Triggering Replication Event
	UPROPERTY(ReplicatedUsing = OnRep_CurrentItemMesh, BlueprintReadOnly, Category = "Saw")
	UStaticMesh* CurrentItemMesh = nullptr;

	// What Mesh Should Be Used For The Log (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saw")
	UStaticMesh* LogMesh;

	// What Mesh Should Be Used For The Bucket (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Saw")
	UStaticMesh* BucketMesh;

	// Reference To The DataTable (Needs To Be Assigned In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UDataTable* ItemDataTable;

	// 'Empty', 'PlayingAnimation', 'HasBucket;
	UPROPERTY(Replicated)
	FName CurrentState = "Empty";


	int32 StoredLogs = 0;

	// Widget Refrence
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	USawBenchWidget* SawBenchWidget;

	// * * * * * * * * * * Functions * * * * * * * * * * 

	// Constructor
	ASawBench();

	// BeginPlay
	virtual void BeginPlay() override;

	// Implement & Override The Interface Function
	virtual void OnInteract_Implementation(AActor* Interactor);

	// Override Set Highlight Function
	virtual void SetHighlight(bool bEnabled, bool bIsClosest = false, AActor* Player = nullptr) override;

	// Override Overlap Function To Check For Items Being Thrown At The Station
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult) override;


	UFUNCTION()
	void PlaceLog();

	UFUNCTION()
	void StartSawing();

	// Multicast RPC to play animations on all clients
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnAnimationStart();

	// Log Placed -> Start Sawing Animation
	// Implemented In Blueprints
	UFUNCTION(BlueprintImplementableEvent, Category = "Saw")
	void OnAnimationStart();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Sawing Animation Is Done
	// Called From On OnAnimationStart() Blueprints
	UFUNCTION(BlueprintCallable, Category = "Saw")
	void OnAnimaitionComplete();


	// Replication Event For Current Item Mesh
	UFUNCTION()
	void OnRep_CurrentItemMesh();
};

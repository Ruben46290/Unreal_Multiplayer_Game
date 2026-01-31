// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrderManager.generated.h"

class AOrderStation;

// Order Recipe Structure
USTRUCT(BlueprintType)
struct FOrder : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PatienceTime = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BasePoints = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* OrderIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString OrderName;
};

USTRUCT(BlueprintType)
struct FCustomers : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StationIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OrderIndex = 0;
};

UCLASS()
class MULTIPLAYER_API AOrderManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOrderManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// * * * * * * * * * * Order Sequence * * * * * * * * * *

	// Array Of All Orders That Will Be Made In The Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orders")
	TArray<FOrder> LevelOrderSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orders")
	TArray<FCustomers> CustomerSequence;

	// Current Order Pos In Sequence
	UPROPERTY(BlueprintReadOnly, Category = "Orders")
	int32 CurrentSpawnIndex = 0;

	//UFUNCTION()
	//void OnRep_CurrentOrderIndex();

	// * * * * * * * * * * Stations * * * * * * * * * *

	// Refrence To All Stations Placed On The Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orders")
	TArray<AOrderStation*> RegisteredStations;


	// * * * * * * * * * * Order Management * * * * * * * * * *

	// Request Next Order (Called by stations)
	UFUNCTION(BlueprintCallable, Category = "Orders")
	bool RequestNextOrder(AOrderStation* RequestingStation);

	


//	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Orders")
//	void ServerCompleteOrder();

	float GameTime = 0.0f;

	float NextSpawnTime = 0.0f;

	bool bIsSpawningEnabled = true;
};

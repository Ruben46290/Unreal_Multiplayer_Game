// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OrderManager.generated.h"

class AOrderStation;

// ENUM For Types Of Items That Can Be Ordered
// Needs To Match With DataTable Row Names
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Apple,
	AppleJuice,
	Banana,
	BananaJuice
};

USTRUCT(BlueprintType)
struct FCustomer : public FTableRowBase
{
	GENERATED_BODY()

	// How Many Seconds Into The Level Does The Customer Spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime = 0.0f;

	// What Station Does The Customer Spawn At
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StationIndex = 0;

	// What Items Does The Customer Want
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EItemType> RequiredItems;

	// How Many Points Will The Customer Gice - 0 = Automatic Pricing ( Normal Item = 2.5, Juice = 5 )
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PointsToGive = 0.0f;

	// How Long Will The Customer Wait In The Line / Station
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TotalPatience = 60.0f;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orders")
	TArray<FCustomer> CustomerSequence;

	// * * * * * * * * * * Stations * * * * * * * * * *

	// Refrence To All Stations Placed On The Map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orders")
	TArray<AOrderStation*> RegisteredStations;


	// * * * * * * * * * * Order Management * * * * * * * * * *

	// Request Next Order (Called by stations)
	UFUNCTION(BlueprintCallable, Category = "Orders")
	void SpawnCustomer(FCustomer CustomerData);


	float GameTime = 0.0f;

	float NextSpawnTime = 0.0f;

	int32 CurrentSpawnIndex = 0;

	bool bIsSpawningEnabled = true;

protected:



	float CalculateOrderPrice(TArray<EItemType> RequiredItems);

	// Refrence To Item Data Table (Needs To Be Set In Editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* ItemDataTable;
};

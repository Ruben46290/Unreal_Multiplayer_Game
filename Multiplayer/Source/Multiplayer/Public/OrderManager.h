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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StationIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OrderIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<EItemType> RequiredItems;
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
};

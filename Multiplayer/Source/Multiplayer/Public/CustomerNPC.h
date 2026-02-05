// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

    #include "CoreMinimal.h"
    #include "GameFramework/Actor.h"
    #include "OrderManager.h" // Include For FOrder
    #include "CustomerNPC.generated.h"

    // Tells Event Distapher Below That ACustomerNPC Class Exists
    class ACustomerNPC; 
    

    // Event Dispatcher F
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReachedStation, ACustomerNPC*, Customer);


    UCLASS()
    class MULTIPLAYER_API ACustomerNPC : public AActor
    {
	    GENERATED_BODY()
	
    public:	
	    // Sets default values for this actor's properties
	    ACustomerNPC();

        // Event Dispatcher Function
        UPROPERTY(BlueprintAssignable, Category = "Events")
        FOnReachedStation OnReachedStation;

        FCustomer StoredCustomerData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// * * * * * * * * * * Movement * * * * * * * * * 

    virtual void Tick(float DeltaTime) override;

    
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsMoving = false;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 200.0f;

    // Array of waypoints to follow (path through queue)
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    TArray<FVector> WaypointPath;

    // Current waypoint index
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    int32 CurrentWaypointIndex = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FVector CurrentTargetPosition;


    UFUNCTION(BlueprintCallable, Category = "Movement")
    void MoveAlongPath(TArray<FVector> Path);

    UFUNCTION()
    void MoveToNextWaypoint();

    // Is This Custom The First One In The Line?
    UPROPERTY(BlueprintReadOnly, Category = "Order")
    bool bIsFirstInLine = false;
};

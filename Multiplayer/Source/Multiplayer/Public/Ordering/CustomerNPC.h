// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

    #include "CoreMinimal.h"
    #include "GameFramework/Actor.h"
    #include "OrderManager.h" // Include For FOrder
    #include "Components/WidgetComponent.h"
    #include "CustomerWidget.h"
    #include "CustomerNPC.generated.h"

    // Tells Event Distapher Below That ACustomerNPC Class Exists
    class ACustomerNPC; 
	class AOrderStation;

    // Event Dispatcher F
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReachedStation, ACustomerNPC*, Customer);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCustomerLeaves, ACustomerNPC*, Customer);


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

        UPROPERTY(Replicated)
        FCustomer StoredCustomerData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    USceneComponent* RootSceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
    TArray<USkeletalMesh*> MeshOptions;

    UPROPERTY(ReplicatedUsing = OnRep_ChosenMesh)
    USkeletalMesh* ChosenMesh;

    // Widget Component
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UWidgetComponent* WidgetComponent;

    UFUNCTION()
    void OnRep_ChosenMesh();

    // Blueprint Events For Animations
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayWalkAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayIdleAnimation();

    bool bWalkingAnimationPlaying = false;

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

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

    // Is This Custom The First One In The Line?
    UPROPERTY(BlueprintReadOnly, Category = "Order")
    bool bIsFirstInLine = false;

    // * * * * * * * * * * Patience * * * * * * * * * 
 protected:

    // How Long Has The Customer Been Waiting
    UPROPERTY(ReplicatedUsing = OnRep_CurrentWaitTime)
    float CurrentWaitTime = 0.0f;

    // Replication Event For Current Wait Time
    UFUNCTION()
    void OnRep_CurrentWaitTime();

    // Timer For Ticking Patience
    FTimerHandle PatienceTimer;

    // How Long Between Patience Timer Ticks
    UPROPERTY()
    float PatienceTickSpeed = 0.1f;

	// Multiplier For How Fast Patience Ticks - Used For Singleplayer To Make It Slower
    UPROPERTY()
	float PatienceMultiplier = 1.0f;

    // Patience Tiemer Tick
    UFUNCTION()
    void PatienceTimerTick();

    // Update Patience Widget
    // Seperate Function So OnRep & Server Can Call Easily
    UFUNCTION()
    void UpdatePatienceUI();


    // * * * * * Patience UI * * * * * 

    UPROPERTY()
    UCustomerWidget* PatienceWidget;

public:
    // Event Dispather For When Customer Leaves At The Station
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnReachedStation OnCustomerLeaves;

    UFUNCTION()
    void ToggleUI(bool bIsVisible);

    UPROPERTY()
    AOrderStation* CurrentStation;


};

// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomerNPC.h"

// Sets default values
ACustomerNPC::ACustomerNPC()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACustomerNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACustomerNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Handle walking to target position
    if (bIsMoving)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector Direction = CurrentTargetPosition - CurrentLocation;

        // Ignore Z difference (only move horizontally)
        Direction.Z = 0;
        float Distance = Direction.Size();

        // Check if we've reached the target
        if (Distance < 2.5f)
        {
            // Snap to exact position
            FVector FinalPosition = CurrentLocation;
            FinalPosition.X = CurrentTargetPosition.X;
            FinalPosition.Y = CurrentTargetPosition.Y;
            SetActorLocation(FinalPosition);

            MoveToNextWaypoint();
        }
        else
        {
            // Move towards target
            Direction.Normalize();
            FVector NewLocation = CurrentLocation + (Direction * WalkSpeed * DeltaTime);
            SetActorLocation(NewLocation);
        }
    }
}


void ACustomerNPC::MoveAlongPath(TArray<FVector> Path)
{
    // If Theres No Path Set - Print & Return
    if (Path.Num() == 0) if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Red, TEXT("Empty path!"));return; }

    // Store Path
    WaypointPath = Path;
    CurrentWaypointIndex = 0;

    // Start Moving
    CurrentTargetPosition = WaypointPath[0];
    bIsMoving = true;

}





void ACustomerNPC::MoveToNextWaypoint()
{
    // Get Next Waypoint Index
    CurrentWaypointIndex++;

    // Is The Path Finished
    if (CurrentWaypointIndex >= WaypointPath.Num()) {

        // Reset Variables
        bIsMoving = false;
        WaypointPath.Empty();
        CurrentWaypointIndex = 0;

        if (bIsFirstInLine) {

            if (GEngine) {
                GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, TEXT("Finished path - Reached station!"));
            }

            // Broadcast Event Dispatcher
            OnReachedStation.Broadcast(this);

        }
        return;
    }

    CurrentTargetPosition = WaypointPath[CurrentWaypointIndex];


}


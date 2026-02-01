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
        FVector Direction = TargetPosition - CurrentLocation;

        // Ignore Z difference (only move horizontally)
        Direction.Z = 0;
        float Distance = Direction.Size();

        // Check if we've reached the target
        if (Distance < 10.0f) // Within 10 units
        {
            // Snap to exact position
            FVector FinalPosition = CurrentLocation;
            FinalPosition.X = TargetPosition.X;
            FinalPosition.Y = TargetPosition.Y;
            SetActorLocation(FinalPosition);

            bIsMoving = false;

            if (GEngine) {
                GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Green, TEXT("Reached queue position"));
            }
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

void ACustomerNPC::MoveToPosition(FVector NewPosition)
{
    TargetPosition = NewPosition;
    bIsMoving = true;

    if (GEngine) {
        GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Yellow,
            FString::Printf(TEXT("Walking to: %s"), *NewPosition.ToString()));
    }
}


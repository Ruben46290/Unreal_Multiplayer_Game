// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomerNPC.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACustomerNPC::ACustomerNPC()
{
    bReplicates = true;
    bAlwaysRelevant = true;

    RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
    RootComponent = RootSceneComponent;

    // Create Skeletal Mesh
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    SkeletalMeshComponent->SetupAttachment(RootSceneComponent);

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACustomerNPC::BeginPlay()
{
	Super::BeginPlay();
	
    // Disable Tick When Spawned In
    // Tick We Be Instanly Enabled When There Is A Path For The NPC To Walk Down
    SetActorTickEnabled(false);

    // Only Run On Server
    if (!HasAuthority()) { return; }

    // If Mesh Options Array Has Been Setup Properly
    if (MeshOptions.Num() != 0) {

        int32 ChosenIndex = FMath::RandRange(0, MeshOptions.Num() - 1);

        // Set Skeletal Mesh To Random Option
        SkeletalMeshComponent->SetSkeletalMesh(MeshOptions[ChosenIndex]);

        // Set Chosen Mesh Variable - Used For Replicating
        ChosenMesh = MeshOptions[ChosenIndex];

    }
}

void ACustomerNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    //if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Red, TEXT("NPC Tick")); }

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
            // Move Towards Target
            Direction.Normalize();
            FVector NewLocation = CurrentLocation + (Direction * WalkSpeed * DeltaTime);
            SetActorLocation(NewLocation);

            // Look Towards Target
            FRotator TargetRotation = Direction.Rotation();
            FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f);
            SetActorRotation(NewRotation);

            // If Walking Animation Isn't Playing
            if (!bWalkingAnimationPlaying)
            {
                // Player Walking Animation
                PlayWalkAnimation();
                bWalkingAnimationPlaying = true;
            }

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

    // Start Tick
    SetActorTickEnabled(true);
}





void ACustomerNPC::MoveToNextWaypoint()
{
    // Get Next Waypoint Index
    CurrentWaypointIndex++;

    // Is The Path Finished
    if (CurrentWaypointIndex >= WaypointPath.Num()) {

        // Stop Tick
        SetActorTickEnabled(false);

        // Reset Variables
        bIsMoving = false;
        WaypointPath.Empty();
        CurrentWaypointIndex = 0;

        if (bIsFirstInLine) {
            // Broadcast Event Dispatcher
            OnReachedStation.Broadcast(this);

        }

        // If Walking Animation Is Playing
        if (bWalkingAnimationPlaying)
        {
            // Play Idle Animation
            PlayIdleAnimation();
            bWalkingAnimationPlaying = false;
        }

        return;
    }

    CurrentTargetPosition = WaypointPath[CurrentWaypointIndex];


}

void ACustomerNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACustomerNPC, StoredCustomerData);
    DOREPLIFETIME(ACustomerNPC, ChosenMesh);
}


void ACustomerNPC::OnRep_ChosenMesh()
{
    if (!ChosenMesh || !SkeletalMeshComponent) { return; }

    SkeletalMeshComponent->SetSkeletalMesh(ChosenMesh);
}
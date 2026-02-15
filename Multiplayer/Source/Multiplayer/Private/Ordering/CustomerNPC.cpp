// Fill out your copyright notice in the Description page of Project Settings.


#include "Ordering/CustomerNPC.h"
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

    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("PatienceWidget"));
    WidgetComponent->SetupAttachment(SkeletalMeshComponent);
    WidgetComponent->SetVisibility(false);

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

    // Set Saved Widget To Widget Components Widget
    PatienceWidget = Cast<UCustomerWidget>(WidgetComponent->GetUserWidgetObject());

    // Only Run On Server
    if (!HasAuthority()) { return; }


    // If Mesh Options Array Has Been Setup Properly
    if (MeshOptions.Num() != 0) {

        // Roll A Random Index First To Use The Same Random Int For Setting The Mesh & Replicated ChosenMesh Var
        int32 ChosenIndex = FMath::RandRange(0, MeshOptions.Num() - 1);

        // Set Skeletal Mesh To Random Option
        SkeletalMeshComponent->SetSkeletalMesh(MeshOptions[ChosenIndex]);

        // Set Chosen Mesh Variable - Used For Replicating
        ChosenMesh = MeshOptions[ChosenIndex];

    }

    // Start Patience Timer
    GetWorldTimerManager().SetTimer(PatienceTimer, this,
        &ACustomerNPC::PatienceTimerTick, PatienceTickSpeed, true);
}

// * * * * * * * * * * * * * * * Movement * * * * * * * * * * * * * * *
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


// * * * * * * * * * * * * * * * Patience * * * * * * * * * * * * * * *

void ACustomerNPC::PatienceTimerTick()
{
    // Only Tick When The NPC Isn't Moving
    if (bIsMoving) { return; }

    // Tick Current Waiting Time Up By Timer Speed
    // e.g Tick Speed = 0.25, Function Is Called Every 0.25 Seconds & Adds 0.25
    CurrentWaitTime += PatienceTickSpeed;

    // Update UI On The Server
    // Only The Server Starts The Timer
    UpdatePatienceUI();

}


void ACustomerNPC::OnRep_CurrentWaitTime()
{
    // Update Patience On The Client
    UpdatePatienceUI();
}


void ACustomerNPC::UpdatePatienceUI()
{
    // If The Customer Has Been Waiting For Too Long
    if (CurrentWaitTime >= StoredCustomerData.TotalPatience) {

        // Broadcast Customer Leaving Event Dispatcher
        OnCustomerLeaves.Broadcast(this);

        // Replace With Walking Off Animation?
        Destroy();
    }

    // Customer Is Still Waiting
    else {

        // If Widget Is Valid
        if (PatienceWidget) {

            // Update UI With Percentage Of How Long The Customer Has Been Waiting
            PatienceWidget->UpdateUI(1 - (CurrentWaitTime / StoredCustomerData.TotalPatience));
        }
    }
}

// Toggle The Patience Widget Visibility
void ACustomerNPC::ToggleUI(bool bIsVisible)
{
    if (bIsVisible) {
        WidgetComponent->SetVisibility(true);
        if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Green, TEXT("Show UI")); }
    }
    else {
        WidgetComponent->SetVisibility(false);
    }
}

// * * * * * * * * * * * * * * * Replication * * * * * * * * * * * * * * *

void ACustomerNPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACustomerNPC, StoredCustomerData);
    DOREPLIFETIME(ACustomerNPC, ChosenMesh);
    DOREPLIFETIME(ACustomerNPC, CurrentWaitTime);
}


void ACustomerNPC::OnRep_ChosenMesh()
{
    if (!ChosenMesh || !SkeletalMeshComponent) { return; }

    SkeletalMeshComponent->SetSkeletalMesh(ChosenMesh);
}
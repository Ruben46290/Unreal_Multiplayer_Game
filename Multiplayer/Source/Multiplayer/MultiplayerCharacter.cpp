// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InteractInterface.h"
#include "Item.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


AMultiplayerCharacter::AMultiplayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Held Item Mesh
	ItemMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ItemMeshComponent->AttachToComponent(GetMesh(),
		FAttachmentTransformRules::KeepRelativeTransform,
		FName("hand_r"));

	// Item Drop Location
	ItemDropLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropLocation"));
	ItemDropLocation->SetupAttachment(Mesh);


	// Trajectory Spline
	TrajectorySpline = CreateDefaultSubobject<USplineComponent>(TEXT("TrajectorySpline"));
	TrajectorySpline->SetupAttachment(RootComponent);
	TrajectorySpline->SetVisibility(false);


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMultiplayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AMultiplayerCharacter::Tick(float DeltaTime)
{
	// If A Throw Is Currently Being Charged
	if (bIsChargingThrow) {

		// Add Delta Time To Current Charge
		CurrentThrowCharge = FMath::Min(CurrentThrowCharge + (ChargeRate * DeltaTime), MaxThrowCharge);

		UpdateTrajectoryVisualization();
	}
}

// * * * * * * * * * * Input * * * * * * * * * *

void AMultiplayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Look);

		// Interact
		EnhancedInputComponent->BindAction(Interact, ETriggerEvent::Started, this, &AMultiplayerCharacter::OnInteractPressed);

		// Drop
		EnhancedInputComponent->BindAction(Drop, ETriggerEvent::Started, this, &AMultiplayerCharacter::DropCurrentItem);

		// Throwing
		EnhancedInputComponent->BindAction(Throw, ETriggerEvent::Started, this, &AMultiplayerCharacter::StartThrowingHeldItem);
		EnhancedInputComponent->BindAction(Throw, ETriggerEvent::Completed, this, &AMultiplayerCharacter::StopThrowingHeldItem);
	}
}

void AMultiplayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMultiplayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//////////////////////////////////////////////////////////////////////////

// * * * * * * * * * * Interaction * * * * * * * * * *
 
// On Interact Button Pressed, Call Server Interact
void AMultiplayerCharacter::OnInteractPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interact Key Pressed!"));

	//// Debugging
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Interact Pressed!"));
	//}

	if (ClosestInteractable) // && IsLocallyControlled?
	{
		Server_Interact(ClosestInteractable);
	}
}


void AMultiplayerCharacter::Server_Interact_Implementation(AActor* ActorToInteract)
{
	if (!ActorToInteract)
	{
		return;
	}
	
	// Call Interact Interface On Chosen Actor
	IInteractInterface::Execute_OnInteract(ActorToInteract, this);

}


//AActor* AMultiplayerCharacter::FindInteractableActor()
//{
//
//	// Get Player Location
//	FVector PlayerLocation = GetActorLocation();
//
//	// Setup Collision Paramaters
//	FCollisionQueryParams QueryParams;
//	QueryParams.AddIgnoredActor(this); // Ignore Self
//
//	// Make An Array To Store Hit Results
//	TArray < FHitResult> HitResults;
//
//	// Sphere Trace
//	bool bHit = GetWorld()->SweepMultiByChannel(
//		HitResults, // Array To Store Hit Results
//		PlayerLocation, // Start Location
//		PlayerLocation, // End Location
//		FQuat::Identity,
//		ECC_Visibility,
//		FCollisionShape::MakeSphere(InteractionRadius),
//		QueryParams // Apply Custom Paramters (Ingore Self)
//	);
//
//	// Draw Debug Sphere
//	//DrawDebugSphere(GetWorld(),PlayerLocation,InteractionRadius,16,bHit ? FColor::Green : FColor::Red,false,2.0f);
//
//
//	// Make A New Pointer For The Closest Object
//	AActor* ClosestInteractableItem = nullptr;
//
//	// Make A New Closest Distance Float, Set To Max Distance Possible / Max Distance Of Sight Sphere
//	float ClosestDistance = InteractionRadius;
//
//	// For Each Hit Actor
//	for (const FHitResult& Hit : HitResults) {
//
//		// Get Current Actor
//		AActor* HitActor = Hit.GetActor();
//
//		// Is The Actor Valid & Does The Actor Have The Interact Interface
//		if (HitActor && HitActor->Implements<UInteractInterface>()) {
//
//			// Set Distance 
//			float Distance = FVector::Dist(PlayerLocation, HitActor->GetActorLocation());
//
//			// Is This Actor The New Closest Actor?
//			if (Distance < ClosestDistance) {
//
//				// Set Closest Distance To Distance
//				ClosestDistance = Distance;
//
//				// Set Closest Actor Refrence To Current Actor
//				ClosestInteractableItem = HitActor;
//
//			}
//
//		}
//
//
//
//	}
//
//	return ClosestInteractableItem;
//}


// * * * * * * * * * * Item Picking Up & Dropping * * * * * * * * * *
void AMultiplayerCharacter::PickupItem(FItemData Item)
{
	//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Yellow, TEXT("Item PickedUp")); }

	// Call server RPC instead of handling locally
	Server_PickupItem(Item);
}

void AMultiplayerCharacter::Server_PickupItem_Implementation(FItemData Item)
{
	// If there's a current held item, drop it first
	if (HeldItem.ItemID != NAME_None) {
		Server_SpawnItem(ItemDropLocation->GetComponentLocation(), HeldItem.ItemID);
	}

	// Set held item (will replicate to all clients)
	HeldItem = Item;

	// Update mesh on server
	ItemMeshComponent->SetStaticMesh(Item.Mesh);


	if (Item.bIsJuice) {
		// Create dynamic material instance
		UMaterialInstanceDynamic* DynamicMat = ItemMeshComponent->CreateDynamicMaterialInstance(1);

		DynamicMat->SetVectorParameterValue(FName("Color"), Item.ItemColor);

	}

}

void AMultiplayerCharacter::DropCurrentItem()
{
	// Call server RPC
	Server_DropCurrentItem();
}

void AMultiplayerCharacter::Server_DropCurrentItem_Implementation()
{
	if (HeldItem.ItemID != NAME_None) {

		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0, FColor::Red,
		//		FString::Printf(TEXT("Dropping ItemID: %s"), *HeldItem.ItemID.ToString()));
		//}

		// Spawn dropped item
		Server_SpawnItem(ItemDropLocation->GetComponentLocation(), HeldItem.ItemID);

		// Clear item data (will replicate to all clients)
		HeldItem = FItemData();

		// Clear mesh on server
		ItemMeshComponent->SetStaticMesh(nullptr);
	}
}

// This function runs on clients when HeldItem replicates
void AMultiplayerCharacter::OnRep_HeldItem()
{
	// Update the mesh on clients when HeldItem changes
	if (HeldItem.ItemID != NAME_None) {
		ItemMeshComponent->SetStaticMesh(HeldItem.Mesh);

		if (HeldItem.bIsJuice) {
			// Create dynamic material instance
			UMaterialInstanceDynamic* DynamicMat = ItemMeshComponent->CreateDynamicMaterialInstance(1);

			DynamicMat->SetVectorParameterValue(FName("Color"), HeldItem.ItemColor);

		}

	}
	else {
		ItemMeshComponent->SetStaticMesh(nullptr);
	}
}

void AMultiplayerCharacter::Server_SpawnItem_Implementation(FVector Location, FName ItemID)
{
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Yellow, TEXT("Drop Item")); }

	// Spawns the actor
	AItem* NewItem = GetWorld()->SpawnActorDeferred<AItem>(
		ItemBlueprintClass,
		FTransform(FRotator::ZeroRotator, Location)
	);

	if (NewItem)
	{
		NewItem->ItemName = ItemID;
		NewItem->FinishSpawning(FTransform(FRotator::ZeroRotator, Location));

		if (NewItem->ItemName == "Juice") {
			//if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Throw Juice")); }
			NewItem->ApplyJuiceColor(HeldItem.ItemColor);
		}

	}
}

void AMultiplayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerCharacter, HeldItem);
}


// * * *  * * * * * * * Interactable Objects Highlighting Functions * * * * * * * * * *

// * * Find The Closest Interactable Object * * 
void AMultiplayerCharacter::UpdateClosestInteractable()
{
	// Setup Variables
	AInteractableActor* NewClosest = nullptr;
	float ClosestDistance = FLT_MAX;
	//FVector PlayerLocation = GetActorLocation();
	FVector PlayerLocation = Mesh->GetComponentLocation();


	// For Each Nearby Interactable
	for (AInteractableActor* Interactable : NearbyInteractables) {

		// If Interactable Is Valid
		if (Interactable) {

			// Get Distance Between Interactable & Player
			float Distance = FVector::Dist(PlayerLocation, Interactable->GetActorLocation());

			// If It's The New Closest Interactable
			if (Distance < ClosestDistance)
			{
				// Save Refrence To Self & Update ClosestDistance
				ClosestDistance = Distance;
				NewClosest = Interactable;
			}

		}

	}

	// Update All Highlights
	for (AInteractableActor* Interactable : NearbyInteractables)
	{
		if (Interactable)
		{
			bool bIsClosest = (Interactable == NewClosest);
			Interactable->SetHighlight(true, bIsClosest);
		}
	}

	ClosestInteractable = NewClosest;

}

void AMultiplayerCharacter::AddNearbyInteractable(AInteractableActor* Interactable)
{
	if (Interactable && !NearbyInteractables.Contains(Interactable))
	{
		NearbyInteractables.Add(Interactable);

		// Start timer when first interactable is added
		if (NearbyInteractables.Num() == 1)
		{
			GetWorldTimerManager().SetTimer(HighlightUpdateTimer, this,
				&AMultiplayerCharacter::UpdateClosestInteractable, 0.1f, true);
		}

		UpdateClosestInteractable();
	}
}

void AMultiplayerCharacter::RemoveNearbyInteractable(AInteractableActor* Interactable)
{
	if (NearbyInteractables.Contains(Interactable))
	{

		NearbyInteractables.Remove(Interactable);

		Interactable->SetHighlight(false, false);

		// Stop timer when no more interactables
		if (NearbyInteractables.Num() == 0)
		{
			GetWorldTimerManager().ClearTimer(HighlightUpdateTimer);
			ClosestInteractable = nullptr;
		}
		else
		{
			UpdateClosestInteractable();
		}
	}
}


// * * * * * * * * * * Throwing * * * * * * * * * *

void AMultiplayerCharacter::StartThrowingHeldItem()
{
	// if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Yellow, TEXT("Throw Item Key Pressed")); }

	// If Theres A Current Held Item
	if (HeldItem.ItemID != NAME_None) {

		// Reset Charging Variables
		bIsChargingThrow = true;
		CurrentThrowCharge = 0.0f;

		// Show Trajectory Spline
		if (TrajectorySpline)
		{
			TrajectorySpline->SetVisibility(true);
		}
	}
}


void AMultiplayerCharacter::StopThrowingHeldItem()
{
	//if (GEngine) {GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Yellow,FString::Printf(TEXT("Throw Item Key Released - Charge: %.2f"), CurrentThrowCharge));}

	if (HeldItem.ItemID != NAME_None && bIsChargingThrow)
	{
		// Calculate Throw Direction
		FVector ThrowDirection = ItemDropLocation->GetForwardVector();

		// Calculate Throw Strength Based On Charge (0.0 to 1.0)
		float ThrowStrength = CurrentThrowCharge;

		// Send To Server
		Server_ThrowItem(ThrowDirection, ThrowStrength);

		// Reset Throw State
		bIsChargingThrow = false;
		CurrentThrowCharge = 0.0f;

		// Hide Trajectory
		if (TrajectorySpline)
		{
			TrajectorySpline->SetVisibility(false);

			// Destroy Trajectory Mesh Componenets
			for (USplineMeshComponent* MeshComp : SplineMeshComponents)
			{
				if (MeshComp)
				{
					MeshComp->DestroyComponent();
				}
			}
		}
	}
}

void AMultiplayerCharacter::Server_ThrowItem_Implementation(FVector ThrowDirection, float ThrowStrength)
{
	if (HeldItem.ItemID == NAME_None)
	{
		return; // No item to throw
	}

	// Calculate throw force based on charge
	float ThrowForce = FMath::Lerp(MinThrowForce, MaxThrowForce, ThrowStrength);

	// Spawn the item
	FVector SpawnLocation = ItemDropLocation->GetComponentLocation();

	AItem* ThrownItem = GetWorld()->SpawnActorDeferred<AItem>(
		ItemBlueprintClass,
		FTransform(FRotator::ZeroRotator, SpawnLocation)
	);

	if (ThrownItem)
	{
		// Set item properties
		ThrownItem->ItemName = HeldItem.ItemID;

		// Finish spawning
		ThrownItem->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnLocation));

		// Apply throw force
		if (ThrownItem->MeshComponent)
		{
			// Make sure physics is enabled
			ThrownItem->MeshComponent->SetSimulatePhysics(true);

			// Apply impulse in the throw direction
			FVector ThrowImpulse = ThrowDirection * ThrowForce;
			ThrownItem->MeshComponent->AddImpulse(ThrowImpulse, NAME_None, true);

			//// Optional: Add some random spin for realism
			//FVector RandomTorque = FVector(
			//	FMath::RandRange(-100.0f, 100.0f),
			//	FMath::RandRange(-100.0f, 100.0f),
			//	FMath::RandRange(-100.0f, 100.0f)
			//) * ThrowStrength;
			//ThrownItem->MeshComponent->AddTorqueInRadians(RandomTorque, NAME_None, true);
		}
	}

	// Clear held item
	HeldItem = FItemData();

	if (ItemMeshComponent)
	{
		ItemMeshComponent->SetStaticMesh(nullptr);
		ItemMeshComponent->MarkRenderStateDirty();
	}
}



// * * * * * * * * * * Throwing Trajectoy Visualization * * * * * * * * * *

void AMultiplayerCharacter::UpdateTrajectoryVisualization()
{
	if (!ItemMeshComponent || HeldItem.ItemID == NAME_None || !TrajectorySpline)
	{
		return;
	}

	FVector StartLocation = ItemDropLocation->GetComponentLocation();
	FVector ThrowDirection = ItemDropLocation->GetForwardVector();
	float ThrowForce = FMath::Lerp(MinThrowForce, MaxThrowForce, CurrentThrowCharge);
	float ItemMass = 1.0f;
	FVector InitialVelocity = ThrowDirection * (ThrowForce / ItemMass);

	FVector CurrentPosition = StartLocation;
	FVector CurrentVelocity = InitialVelocity;
	FVector Gravity = FVector(0, 0, GetWorld()->GetGravityZ());

	// Clear existing spline points
	TrajectorySpline->ClearSplinePoints();

	bool bHitGround = false;

	for (int32 i = 0; i < TrajectorySteps; i++)
	{
		// Add current position to spline
		TrajectorySpline->AddSplinePoint(CurrentPosition, ESplineCoordinateSpace::World, false);

		FVector NextPosition = CurrentPosition + (CurrentVelocity * TrajectoryTimeStep);

		// Check for ground hit
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, CurrentPosition, NextPosition, ECC_Visibility))
		{
			// Add the hit point as final spline point
			TrajectorySpline->AddSplinePoint(HitResult.Location, ESplineCoordinateSpace::World, false);

			// Draw landing marker
			if (bShowLandingMarker)
			{
				DrawDebugSphere(GetWorld(), HitResult.Location, 20.0f, 12, FColor::Red, false, -1.0f, 0, 2.0f);
			}

			bHitGround = true;
			break;
		}

		CurrentVelocity += Gravity * TrajectoryTimeStep;
		CurrentPosition = NextPosition;
	}

	// If we didn't hit anything, add the final position
	if (!bHitGround)
	{
		TrajectorySpline->AddSplinePoint(CurrentPosition, ESplineCoordinateSpace::World, false);
	}

	// Update the spline to recalculate curves
	TrajectorySpline->UpdateSpline();

	// Make sure spline is visible
	TrajectorySpline->SetVisibility(true);

	if (TrajectoryMesh)
	{
		UpdateSplineMeshes();
	}
}


void AMultiplayerCharacter::UpdateSplineMeshes()
{
	// Clear old mesh components
	for (USplineMeshComponent* MeshComp : SplineMeshComponents)
	{
		if (MeshComp)
		{
			MeshComp->DestroyComponent();
		}
	}
	SplineMeshComponents.Empty();

	int32 NumSegments = TrajectorySpline->GetNumberOfSplinePoints() - 1;

	for (int32 i = 0; i < NumSegments; i++)
	{


		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		SplineMesh->SetStaticMesh(TrajectoryMesh);
		SplineMesh->SetMobility(EComponentMobility::Movable);
		SplineMesh->RegisterComponent();
		SplineMesh->AttachToComponent(TrajectorySpline, FAttachmentTransformRules::KeepRelativeTransform);

		FVector StartPos, StartTangent, EndPos, EndTangent;
		TrajectorySpline->GetLocationAndTangentAtSplinePoint(i, StartPos, StartTangent, ESplineCoordinateSpace::Local);
		TrajectorySpline->GetLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);

		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);

		SplineMeshComponents.Add(SplineMesh);

		// Apply Material
		if (TrajectoryMaterial)
		{
			SplineMesh->SetMaterial(0, TrajectoryMaterial);
		}
	}
}


// * * * * * * * * * * Helper Functions * * * * * * * * * *
FName AMultiplayerCharacter::GetHeldItemName()
{
	// If HeldItem Is Valid
	if (HeldItem.ItemID != NAME_None) {

		// Return Held Item Name
		return HeldItem.ItemID;
	}

	// Return As Blank
	return FName();
}

void AMultiplayerCharacter::ClearHeldItem_Implementation()
{
	// Skip If No Held Item
	if (HeldItem.ItemID == NAME_None)
	{
		return;
	}

	// Clear Held Item
	HeldItem = FItemData();

	// Clear Item Mesh
	if (ItemMeshComponent)
	{
		ItemMeshComponent->SetStaticMesh(nullptr);
		ItemMeshComponent->MarkRenderStateDirty();
	}
}
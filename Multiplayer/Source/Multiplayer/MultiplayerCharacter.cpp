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
#include "DrawDebugHelpers.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMultiplayerCharacter

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMultiplayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

//////////////////////////////////////////////////////////////////////////
// Input

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

		EnhancedInputComponent->BindAction(Interact, ETriggerEvent::Started, this, &AMultiplayerCharacter::OnInteractPressed);
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

void AMultiplayerCharacter::OnInteractPressed()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interact Key Pressed!"));

	//// Debugging
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Interact Pressed!"));
	//}

	if (IsLocallyControlled())
	{
		Server_Interact();
	}
}


void AMultiplayerCharacter::Server_Interact_Implementation()
{
	//// Debugging
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Server Interact Called"));
	//}

	// Find Closest Interactable Actor
	AActor* InteractableActor = FindInteractableActor();

	// If An Actor (With Interact Interface) Is Found
	if (InteractableActor) {
		
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Interactable Object Found"));

		// Call Interact Interface On Chosen Actor
		IInteractInterface::Execute_OnInteract(InteractableActor, this);
	}

}

AActor* AMultiplayerCharacter::FindInteractableActor()
{

	// Get Player Location
	FVector PlayerLocation = GetActorLocation();

	// Setup Collision Paramaters
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Ignore Self

	// Make An Array To Store Hit Results
	TArray < FHitResult> HitResults;

	// Sphere Trace
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults, // Array To Store Hit Results
		PlayerLocation, // Start Location
		PlayerLocation, // End Location
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(InteractionRadius),
		QueryParams // Apply Custom Paramters (Ingore Self)
	);

	//// Debug Visualization
	//if (bDebugInteraction)
	//{
	//	DrawDebugSphere(
	//		GetWorld(),
	//		PlayerLocation,
	//		InteractionRadius,
	//		16,
	//		bHit ? FColor::Green : FColor::Red,
	//		false,
	//		2.0f
	//	);
	//}

	// Make A New Pointer For The Closest Object
	AActor* ClosestInteractable = nullptr;

	// Make A New Closest Distance Float, Set To Max Distance Possible / Max Distance Of Sight Sphere
	float ClosestDistance = InteractionRadius;

	for (const FHitResult& Hit : HitResults) {

		// Get Current Actor
		AActor* HitActor = Hit.GetActor();

		// Is The Actor Valid & Does The Actor Have The Interact Interface
		if (HitActor && HitActor->Implements<UInteractInterface>()) {

			// Set Distance 
			float Distance = FVector::Dist(PlayerLocation, HitActor->GetActorLocation());

			// Is This Actor The New Closest Actor?
			if (Distance < ClosestDistance) {

				// Set Closest Distance To Distance
				ClosestDistance = Distance;

				// Set Closest Actor Refrence To Current Actor
				ClosestInteractable = HitActor;

			}

		}



	}

	return ClosestInteractable;
}



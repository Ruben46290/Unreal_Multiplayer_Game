// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActor.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create Mesh Component
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	// Create sphere collision
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(SphereRadius);

	bReplicates = true;

}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	// Bind Sphere Overlap Events
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::OnOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OnOverlapEnd);
}



void AInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Check if it's a character
	ACharacter* Character = Cast<ACharacter>(OtherActor);

	// Is LocallyControlled() makes this code run only on the machine controlling this character
	// Server & Other clients won't run this
	if (Character && Character->IsLocallyControlled())
	{

		// Save reference to overlapping player
		OverlappingPlayer = Character;

		// Show highlight locally (no RPC needed since we check IsLocallyControlled)
		MeshComponent->SetOverlayMaterial(HighlightMaterial);;
	}
}



void AInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// Check if it's a character
	ACharacter* Character = Cast<ACharacter>(OtherActor);


	// Is LocallyControlled() makes this code run only on the machine controlling this character
	// Server & Other clients won't run this
	if (Character && Character->IsLocallyControlled())
	{

		// Only remove highlight if this is the overlapping player
		if (Character == OverlappingPlayer)
		{
			// Remove highlight locally
			MeshComponent->SetOverlayMaterial(nullptr);

			// Clear reference
			OverlappingPlayer = nullptr;
		}
	}

}

void AInteractableActor::OnInteract_Implementation(AActor* Interactor)
{
	Destroy();
}
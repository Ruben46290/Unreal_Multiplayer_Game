// Fill out your copyright notice in the Description page of Project Settings.


#include "Juicer.h"

AJuicer::AJuicer()
{
	// Create Handle Mesh And Attach To Main Mesh
	HandleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandleMesh"));
	HandleMeshComponent->SetupAttachment(MeshComponent);
}



void AJuicer::OnInteract_Implementation(AActor* Interactor)
{
	
	if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Interact Pressed")); }

	// Cast To Player Class
	AMultiplayerCharacter* Character = Cast<AMultiplayerCharacter>(Interactor);

	if (ItemNeeded == Character->GetHeldItemName()) {

		if (ItemNeeded == "Bucket") {

			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Green, TEXT("Bucket Placed")); }

			Character->ClearHeldItem();

		}

	}
	else {
		if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 2.0, FColor::Blue, TEXT("Item Doesn't Match")); }
	}

}

void AJuicer::SetHighlight(bool bEnabled, bool bIsClosest)
{
	// If Mesh Is Valid
	if (MeshComponent)
	{
		// Is Highlight Enabled
		if (bEnabled)
		{
			// Green For Closest, White For Others
			MeshComponent->SetOverlayMaterial(bIsClosest ? GreenHighlightMaterial : WhiteHighlightMaterial);
			HandleMeshComponent->SetOverlayMaterial(bIsClosest ? GreenHighlightMaterial : WhiteHighlightMaterial);
		}
		else
		{
			MeshComponent->SetOverlayMaterial(nullptr);
			HandleMeshComponent->SetOverlayMaterial(nullptr);
		}
	}
}

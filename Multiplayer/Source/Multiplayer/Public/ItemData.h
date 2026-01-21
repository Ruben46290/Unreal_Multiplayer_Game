// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY();
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanBeJuiced;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsJuice;

	// Colour For Juice
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor ItemColor;
};
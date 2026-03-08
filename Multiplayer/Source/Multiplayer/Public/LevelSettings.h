// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSettings.generated.h"

UCLASS()
class MULTIPLAYER_API ALevelSettings : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelSettings();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	// How Many Seconds Does The Level Last?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Settings")
	float GameLength = 60.f;

	// Score Miltstones - Used At End Of Game To Calculate Stars Earned - [0] = 1 star, [1] = 2 stars, etc.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level Settings")
	TArray<float> ScoreMilestones = { 10.f, 20.f, 30.f, 40.f, 50.f };

};

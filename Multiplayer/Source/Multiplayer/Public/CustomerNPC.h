// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CustomerNPC.generated.h"

UCLASS()
class MULTIPLAYER_API ACustomerNPC : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACustomerNPC();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// * * * * * * * * * * Movement * * * * * * * * * 

    virtual void Tick(float DeltaTime) override;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FVector TargetPosition;

    
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsMoving = false;

    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 200.0f;
    

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void MoveToPosition(FVector NewPosition);


};

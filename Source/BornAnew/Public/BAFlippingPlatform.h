// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BAFlippingPlatform.generated.h"

UCLASS()
class BORNANEW_API ABAFlippingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABAFlippingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	/** If TRUE, the platform flips on its Z axis
	If FALSE, the platform flips on its X axis*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bFlipOnZ;

	/** If the platform is constantly spinning or if there is a pause in between rotations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bIsConstantlyFlipping;

	/** If the platform will rotate in a single direction or rotate back and forth
	WARNING: Ignored if bIsConstantlySpinning is equal to TRUE*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bPingPong;

	/** How much time in seconds is there is in between the movements of the platform
	WARNING: Ignored if bIsConstantlySpinning is equal to TRUE*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float TimeBetweenFlips;

	/** How quickly the platform will spin every tick */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float FlipSpeed;

	/** How much in degrees you want to rotate the platform per phases
	WARNING: Ignored if bIsConstantlySpinning is equal to TRUE*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float FlipTargetAmount;

	float PingPongFlipAmount;

	bool bPing;

	bool bCanFlip;

	float CurrentFlipAmount;

	float TargetFlipAmount;

	FRotator OrigRotation;

	FRotator TargetRotation;

	UFUNCTION()
	void EnableMovement();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BASpinningPlatform.generated.h"

UCLASS()
class BORNANEW_API ABASpinningPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABASpinningPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	/** If the platform is constantly spinning or if there is a pause in between rotations */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bIsConstantlySpinning;

	/** If the platform will rotate in a single direction or rotate back and forth 
	WARNING: Ignored if bIsConstantlySpinning is equal to TRUE*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	bool bPingPong;

	/** How much time in seconds is there is in between the movements of the platform
	WARNING: Ignored if bIsConstantlySpinning is equal to TRUE*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float TimeBetweenSpins;

	/** How quickly the platform will spin every tick */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float SpinSpeed;

	/** How much in degrees you want to rotate the platform per phases
	WARNING: Ignored if bIsConstantlySpinning is equal to TRUE*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float SpinTargetAmount;

	float PingPongSpinAmount;

	bool bPing;

	bool bCanSpin;

	float CurrentSpinAmount;

	float TargetSpinAmount;

	FRotator OrigRotation;

	FRotator TargetRotation;

	UFUNCTION()
	void EnableSpin();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

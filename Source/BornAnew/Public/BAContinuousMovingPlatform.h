// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BAContinuousMovingPlatform.generated.h"

UCLASS()
class BORNANEW_API ABAContinuousMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABAContinuousMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* WaypointPosition;

	/** How fast the platform will move */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float PlatformSpeed;

	/** How long a platform will stay at a waypoint for before moving on to the next waypoint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float TimeAtWaypoint;

	FVector TargetPosition;

	FVector OrigPosition;

	bool bCanMove;

	UFUNCTION()
	void EnableMovement();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

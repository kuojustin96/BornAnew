// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BAMovingPlatform.generated.h"

UCLASS()
class BORNANEW_API ABAMovingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABAMovingPlatform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* TargetPositionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* HitboxComp;

	/** Time until the platform will start moving when the player first touches the platform */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float BeginOverlapDelayTime;

	/** Time until the platform will start moving when the player stops touching the platform */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float EndOverlapDelayTime;

	/** Constant speed the platform will move at towards the target position */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float MovementSpeedToTarget;

	/** Constant speed the platform will move at towards the initial position */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	float MovementSpeedToInitialPosition;

	bool bCanTrigger;

	bool bPlayerGotOff;

	FVector InitalMeshPosition;

	UFUNCTION()
	void OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHitboxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void MoveToTargetPosition();

	UFUNCTION()
	void ReturnToInitialPosition();

	FTimerHandle MovementDelayTimerHandle;
};

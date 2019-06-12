// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BACinematicCameraCut.generated.h"

UCLASS()
class BORNANEW_API ABACinematicCameraCut : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABACinematicCameraCut();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior")
	bool bCanTriggerMultipleTimes;

	bool bHasBeenTriggered;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

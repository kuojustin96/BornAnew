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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behavior")
	bool bCanTriggerMultipleTimes;

	bool bHasBeenTriggered;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CutBackToPlayerCameraPosition();

	UFUNCTION()
	void ReEnablePlayerMovementInput();

	/** Time it takes the player camera to cut to the CameraComp */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraBlendTODuration;

	/** Time it takes the player camera to cut from the CameraComp to the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CameraBlendFROMDuration;

	/** Time camera will stay at the CameraComp location
	WARNING: Ignored if there is dialogue in the cutscene*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float CutDuration;

	class APlayerController* PlayerController;

	class ABAPlayerCharacter* PlayerCharacter;
};

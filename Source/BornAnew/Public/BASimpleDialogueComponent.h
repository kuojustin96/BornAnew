// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "BASimpleDialogueComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BORNANEW_API UBASimpleDialogueComponent : public USphereComponent
{
	GENERATED_BODY()

	UBASimpleDialogueComponent();

protected:

	virtual void BeginPlay() override;

	void LateBeginPlay();

	UPROPERTY(EditAnywhere, Category = "Dialogue")
	class UBASimpleDialogueDataAsset* DialogueDataAsset;

	UPROPERTY(EditAnywhere)
	class UWidgetComponent* DialogueWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	float PlaybackSpeed;

	virtual void OnComponentCreated() override;

	virtual void OnComponentDestroyed(bool bPromoteChildren) override;

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

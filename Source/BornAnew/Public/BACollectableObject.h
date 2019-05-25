// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BACollectableObject.generated.h"

UCLASS()
class BORNANEW_API ABACollectableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABACollectableObject();

	UFUNCTION(BlueprintNativeEvent)
	void OnCollected();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LifespanAfterCollection;
};

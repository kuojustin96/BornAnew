// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BASimpleDialogueDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class BORNANEW_API UBASimpleDialogueDataAsset : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Dialogue;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BAPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BORNANEW_API UBAPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsWalkingSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsJumping;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsSliding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsFalling;
};

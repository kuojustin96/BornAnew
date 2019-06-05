// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BASimpleDialogueWidget.generated.h"

/**
 * 
 */
UCLASS()
class BORNANEW_API UBASimpleDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void EnableUI(float PlaybackSpeed);

	UFUNCTION(BlueprintCallable)
	void DisableUI(float PlaybackSpeed);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
	class UTextBlock* DialogueText;

	void SetDialogueText(FText Text);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UWidgetAnimation* FadeAnimation;

	void TurnOffUI();

	FTimerHandle FadeOutTimerHandle;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BADialogueWidget.generated.h"

/**
 * 
 */
UCLASS()
class BORNANEW_API UBADialogueWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void EnableDialogueUI(class ABACinematicCameraCut* CinCamCut, class UBASimpleDialogueDataAsset* DialogueDataAsset, bool bCanTriggerMultipleTimes);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnInteract();

	UFUNCTION()
	void SetupWidgetComponents();

	void DisableDialogueUI();

	UFUNCTION()
	void SetDialogue();

	bool bHasMoreDialogue;

	bool bSetFullDialogue;

	bool bCanTriggerMultiple;

	int32 SubstringLength;

	int32 CurrentDialogueArrayInt;

	FString TargetDialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class URichTextBlock* RichTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UImage* InteractButtonImage;

	class UBASimpleDialogueDataAsset* DialogueData;

	class ABACinematicCameraCut* CinematicCamCut;
};

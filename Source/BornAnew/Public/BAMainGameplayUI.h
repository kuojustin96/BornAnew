// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BAMainGameplayUI.generated.h"

/**
 * 
 */
UCLASS()
class BORNANEW_API UBAMainGameplayUI : public UUserWidget
{
	GENERATED_BODY()

	UBAMainGameplayUI(const FObjectInitializer& ObjectInitializer);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UImage* CollectableUIImage;

	class UCanvasPanelSlot* CollectableUIImageCanvasSlot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UImage* CollectableCounterUI;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "0.001", ClampMax = "1.0"))
	float BezierCurveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (ClampMin = "0.0"))
	float RandomUnitVectorCircleRadius;

	FVector2D CollectableScreenPosition;

	FVector2D CollectableCounterUIPosition;

	FVector2D BezierPoint1;

	FVector2D BezierPoint2;

	float CurrentStepValue;

	UFUNCTION(BlueprintCallable)
	void OnCollectablePickedUp(FVector CollectablePosition);

	UFUNCTION(BlueprintCallable)
	void CalculateInitialBezierCurvePoints(FVector2D StartPoint, FVector2D EndPoint, FVector2D &BezPoint1, FVector2D &BezPoint2);

	UFUNCTION(BlueprintCallable)
	FVector2D GetPointOnBezierCurve(FVector2D p0, FVector2D p1, FVector2D p2, FVector2D p3, float t);

	UFUNCTION()
	void MoveCollectableUIAlongCurve();
};

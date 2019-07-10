// Fill out your copyright notice in the Description page of Project Settings.


#include "BAMainGameplayUI.h"
#include "BAPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "RandomStream.h"
#include "TimerManager.h"


UBAMainGameplayUI::UBAMainGameplayUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RandomUnitVectorCircleRadius = 300.0f;
	BezierCurveSpeed = 0.05f;
	CurrentStepValue = 0.0f;
}


void UBAMainGameplayUI::NativeConstruct()
{
	Super::NativeConstruct();

	ABAPlayerCharacter* PlayerChar = Cast<ABAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerChar != nullptr)
	{
		PlayerChar->CollectablePickedUp.AddDynamic(this, &UBAMainGameplayUI::OnCollectablePickedUp);
	}

	if (CollectableCounterUI != nullptr)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CollectableCounterUI->Slot);
		CollectableCounterUIPosition = CanvasSlot->GetPosition();
	}

	if (CollectableUIImage != nullptr)
	{
		CollectableUIImageCanvasSlot = Cast<UCanvasPanelSlot>(CollectableUIImage->Slot);
		CollectableUIImage->SetVisibility(ESlateVisibility::Hidden);
	}
}


void UBAMainGameplayUI::OnCollectablePickedUp(FVector CollectablePosition)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController != nullptr && CollectableUIImageCanvasSlot != nullptr)
	{
		FVector2D ScreenPos;
		PlayerController->ProjectWorldLocationToScreen(CollectablePosition, ScreenPos);

		int32 width;
		int32 height;
		PlayerController->GetViewportSize(width, height);
		ScreenPos.X /= width;
		ScreenPos.Y /= height;

		CollectableScreenPosition = ScreenPos;

		CalculateInitialBezierCurvePoints(ScreenPos, CollectableCounterUIPosition, BezierPoint1, BezierPoint2);
		CollectableUIImage->SetVisibility(ESlateVisibility::Visible);

		FTimerDelegate TimerDelgate;
		TimerDelgate.BindUFunction(this, FName("MoveCollectableUIAlongCurve"));
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelgate);
	}
}


void UBAMainGameplayUI::CalculateInitialBezierCurvePoints(FVector2D StartPosition, FVector2D EndPosition, FVector2D &BezPoint1, FVector2D &BezPoint2)
{
	FRandomStream RandStream;
	RandStream.GenerateNewSeed();

	FVector p1UnitVector = RandStream.GetUnitVector() * RandomUnitVectorCircleRadius;
	FVector2D p1UnitVector2D = FVector2D(p1UnitVector.X, p1UnitVector.Y);

	BezPoint1 = p1UnitVector2D + StartPosition;

	bool bYGreater = BezPoint1.Y > StartPosition.Y;
	bool bFindingPoint = true;
	FVector2D MiddlePoint = (StartPosition + EndPosition) / 2;

	while (bFindingPoint == true)
	{
		RandStream.GenerateNewSeed();

		FVector p2UnitVector = RandStream.GetUnitVector() * RandomUnitVectorCircleRadius;
		FVector2D p2UnitVector2D = FVector2D(p2UnitVector.X, p2UnitVector.Y);

		BezPoint2 = p2UnitVector2D + MiddlePoint;

		if ((bYGreater == true && BezPoint2.Y > MiddlePoint.Y) || (bYGreater == false && BezPoint2.Y < MiddlePoint.Y))
		{
			bFindingPoint = false;
		}
	}
}


FVector2D UBAMainGameplayUI::GetPointOnBezierCurve(FVector2D p0, FVector2D p1, FVector2D p2, FVector2D p3, float t)
{
	//Source: https://denisrizov.com/2016/06/02/bezier-curves-unity-package-included/
	FVector2D a = FMath::Lerp(p0, p1, t);
	FVector2D b = FMath::Lerp(p1, p2, t);
	FVector2D c = FMath::Lerp(p2, p3, t);

	FVector2D d = FMath::Lerp(a, b, t);
	FVector2D e = FMath::Lerp(b, c, t);

	FVector2D PointOnCurve = FMath::Lerp(d, e, t);

	return PointOnCurve;
}


void UBAMainGameplayUI::MoveCollectableUIAlongCurve()
{
	FVector2D Position = GetPointOnBezierCurve(CollectableScreenPosition, BezierPoint1, BezierPoint2, CollectableCounterUIPosition, CurrentStepValue);
	CollectableUIImageCanvasSlot->SetPosition(Position);

	CurrentStepValue += BezierCurveSpeed;

	if (CurrentStepValue < 1.0f)
	{
		FTimerDelegate TimerDelgate;
		TimerDelgate.BindUFunction(this, FName("MoveCollectableUIAlongCurve"));
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelgate);
	}
	else
	{
		CollectableUIImage->SetVisibility(ESlateVisibility::Hidden);
		CurrentStepValue = 0.0f;
	}
}
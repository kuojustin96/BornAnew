// Fill out your copyright notice in the Description page of Project Settings.


#include "BAMainGameplayUI.h"
#include "BAPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Components/PanelSlot.h"
#include "Components/CanvasPanelSlot.h"
#include "RandomStream.h"
#include "TimerManager.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"


UBAMainGameplayUI::UBAMainGameplayUI(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RandomUnitVectorCircleRadius = 300.0f;
	BezierCurveSpeed = 0.05f;
	UICollectableCounter = 0;
}


void UBAMainGameplayUI::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<ABAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->CollectablePickedUp.AddDynamic(this, &UBAMainGameplayUI::OnCollectablePickedUp);
	}

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CollectableCounterUI->Slot);
	CollectableCounterUIPosition = CanvasSlot->GetPosition();

	UCanvasPanelSlot* CollectableUIIconCanvasSlot = Cast<UCanvasPanelSlot>(CollectableUIIcon->Slot);
	CollectableIconSlots.Enqueue(CollectableUIIconCanvasSlot);
	CollectableUIIconCanvasSlot->Content->SetVisibility(ESlateVisibility::Hidden);

	CollectableCounterText->SetText(FText::FromString(FString("0")));
}


void UBAMainGameplayUI::OnCollectablePickedUp(FVector CollectablePosition)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController != nullptr)
	{
		UCanvasPanelSlot* WidgetPanel;
		if (CollectableIconSlots.IsEmpty() == false)
		{
			CollectableIconSlots.Dequeue(WidgetPanel);
		}
		else
		{
			if (CollectableUIIconWdigetClass != nullptr)
			{
				UUserWidget* Widget = WidgetTree->ConstructWidget<UUserWidget>(CollectableUIIconWdigetClass);
				MainCanvas->AddChild(Widget);

				UCanvasPanelSlot* WidgetPanelSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
				WidgetPanel = WidgetPanelSlot;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NO WIDGET CLASS GIVEN"));
				return;
			}
		}

		FVector2D ScreenPos;
		PlayerController->ProjectWorldLocationToScreen(CollectablePosition, ScreenPos);

		FVector2D CollectableScreenPosition = ScreenPos * 1.5f;

		CalculateInitialBezierCurvePoints(ScreenPos, CollectableCounterUIPosition, BezierPoint1, BezierPoint2);
		WidgetPanel->Content->SetVisibility(ESlateVisibility::Visible);
		OnCollectablePickedUpExtras(WidgetPanel->Content);

		float CurrentStepValue = 0.0f;

		FTimerDelegate TimerDelgate;
		TimerDelgate.BindUFunction(this, FName("MoveCollectableUIAlongCurve"), WidgetPanel, CollectableScreenPosition, CurrentStepValue);
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


void UBAMainGameplayUI::MoveCollectableUIAlongCurve(UCanvasPanelSlot* PanelSlot, FVector2D CollectableScreenPosition, float CurrentStepValue)
{
	FVector2D Position = GetPointOnBezierCurve(CollectableScreenPosition, BezierPoint1, BezierPoint2, CollectableCounterUIPosition, CurrentStepValue);
	PanelSlot->SetPosition(Position);

	CurrentStepValue += BezierCurveSpeed;

	if (CurrentStepValue < 1.0f)
	{
		FTimerDelegate TimerDelgate;
		TimerDelgate.BindUFunction(this, FName("MoveCollectableUIAlongCurve"), PanelSlot, CollectableScreenPosition, CurrentStepValue);
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelgate);
	}
	else
	{
		PanelSlot->Content->SetVisibility(ESlateVisibility::Hidden);

		//Return widget to queue
		CollectableIconSlots.Enqueue(PanelSlot);

		UICollectableCounter++;
		FText CollectableCount = FText::FromString(FString::FromInt(UICollectableCounter));
		CollectableCounterText->SetText(CollectableCount);
	}
}
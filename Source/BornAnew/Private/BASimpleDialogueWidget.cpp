// Fill out your copyright notice in the Description page of Project Settings.


#include "BASimpleDialogueWidget.h"
#include "WidgetAnimation.h"
#include "TimerManager.h"
#include "TextBlock.h"


void UBASimpleDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TurnOffUI();
}


void UBASimpleDialogueWidget::SetDialogueText(FText Text)
{
	if (DialogueText != nullptr)
	{
		DialogueText->SetText(Text);
	}
}


void UBASimpleDialogueWidget::EnableUI(float PlaybackSpeed)
{
	SetVisibility(ESlateVisibility::Visible);

	if (FadeAnimation != nullptr)
	{
		if (PlaybackSpeed > 0.0f)
		{
			GetWorld()->GetTimerManager().ClearTimer(FadeOutTimerHandle);
			PlayAnimationForward(FadeAnimation, PlaybackSpeed);
		}
	}
}


void UBASimpleDialogueWidget::DisableUI(float PlaybackSpeed)
{
	if (FadeAnimation != nullptr)
	{
		if (PlaybackSpeed > 0.0f)
		{
			PlayAnimationReverse(FadeAnimation, PlaybackSpeed);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(FadeOutTimerHandle, this, &UBASimpleDialogueWidget::TurnOffUI, FadeAnimation->GetEndTime(), false);
}


void UBASimpleDialogueWidget::TurnOffUI()
{
	SetVisibility(ESlateVisibility::Hidden);
}
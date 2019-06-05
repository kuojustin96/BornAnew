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
	UE_LOG(LogTemp, Warning, TEXT("PREPPING SET TEXT"));
	if (DialogueText != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SETTING TEXT"));
		DialogueText->SetText(Text);
	}
}


void UBASimpleDialogueWidget::EnableUI(float PlaybackSpeed)
{
	//Need to check if you need to reset animation if player goes in and out quickly
	SetVisibility(ESlateVisibility::Visible);

	if (FadeAnimation != nullptr)
	{
		if (PlaybackSpeed > 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Playing Fade In Animation"));
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
	UE_LOG(LogTemp, Warning, TEXT("Hid UI"));
	SetVisibility(ESlateVisibility::Hidden);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BASimpleDialogueComponent.h"
#include "Components/WidgetComponent.h"
#include "BASimpleDialogueWidget.h"
#include "BASimpleDialogueDataAsset.h"
#include "TimerManager.h"


UBASimpleDialogueComponent::UBASimpleDialogueComponent()
{
	InitSphereRadius(500.0f);

	OnComponentBeginOverlap.AddDynamic(this, &UBASimpleDialogueComponent::OnSphereBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UBASimpleDialogueComponent::OnSphereEndOverlap);

	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);

	DialogueWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Dialogue Widget"));
	DialogueWidget->SetWidgetSpace(EWidgetSpace::Screen);

	PlaybackSpeed = 2.0f;
}


void UBASimpleDialogueComponent::OnComponentCreated()
{
	DialogueWidget->SetupAttachment(GetAttachParent());
}


void UBASimpleDialogueComponent::OnComponentDestroyed(bool bPromoteChildren)
{
	Super::OnComponentDestroyed(bPromoteChildren);

	if (DialogueWidget != nullptr)
	{
		DialogueWidget->DestroyComponent();
	}
}


void UBASimpleDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle BeginPlayTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(BeginPlayTimerHandle, this, &UBASimpleDialogueComponent::LateBeginPlay, 0.01f, false);
}


void UBASimpleDialogueComponent::LateBeginPlay()
{
	if (DialogueWidget != nullptr)
	{
		UBASimpleDialogueWidget* DialogueComp = Cast<UBASimpleDialogueWidget>(DialogueWidget->GetUserWidgetObject());
		if (DialogueComp != nullptr)
		{
			if (DialogueDataAsset != nullptr)
			{
				DialogueComp->SetDialogueText(DialogueDataAsset->Dialogue);
			}
		}
	}
}


void UBASimpleDialogueComponent::OnSphereBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (DialogueWidget != nullptr)
	{
		UBASimpleDialogueWidget* DialogueComp = Cast<UBASimpleDialogueWidget>(DialogueWidget->GetUserWidgetObject());
		if (DialogueComp != nullptr)
		{
			DialogueComp->EnableUI(PlaybackSpeed);
		}
	}
}


void UBASimpleDialogueComponent::OnSphereEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (DialogueWidget != nullptr)
	{
		UBASimpleDialogueWidget* DialogueComp = Cast<UBASimpleDialogueWidget>(DialogueWidget->GetUserWidgetObject());
		if (DialogueComp != nullptr)
		{
			DialogueComp->DisableUI(PlaybackSpeed);
		}
	}
}
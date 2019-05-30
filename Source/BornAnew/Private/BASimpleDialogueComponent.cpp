// Fill out your copyright notice in the Description page of Project Settings.


#include "BASimpleDialogueComponent.h"


UBASimpleDialogueComponent::UBASimpleDialogueComponent()
{
	InitSphereRadius(500.0f);
}


void UBASimpleDialogueComponent::OnSphereBeginOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}


void UBASimpleDialogueComponent::OnSphereEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
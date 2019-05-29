// Fill out your copyright notice in the Description page of Project Settings.


#include "BASimpleDialogueComponent.h"
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UBASimpleDialogueComponent::UBASimpleDialogueComponent()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("Dialogue Sphere Comp"));
	SphereComp->SetupAttachment(this);

	SphereComp->InitSphereRadius(500.0f);
}


// Called when the game starts
void UBASimpleDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UBASimpleDialogueComponent::OnUnregister()
{
	Super::OnUnregister();

	SphereComp->DestroyComponent();
}


void UBASimpleDialogueComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}


void UBASimpleDialogueComponent::OnSphereEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
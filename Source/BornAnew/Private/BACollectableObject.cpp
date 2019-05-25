// Fill out your copyright notice in the Description page of Project Settings.


#include "BACollectableObject.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABACollectableObject::ABACollectableObject()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetGenerateOverlapEvents(true);
	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void ABACollectableObject::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABACollectableObject::OnObjectPickedUp_Implementation()
{

}
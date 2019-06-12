// Fill out your copyright notice in the Description page of Project Settings.


#include "BACinematicCameraCut.h"
#include "Components/BoxComponent.h"

// Sets default values
ABACinematicCameraCut::ABACinematicCameraCut()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABACinematicCameraCut::OnTriggerBeginOverlap);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	RootComponent = TriggerBox;

	bCanTriggerMultipleTimes = false;
	bHasBeenTriggered = false;
}

// Called when the game starts or when spawned
void ABACinematicCameraCut::BeginPlay()
{
	Super::BeginPlay();
	
}


void ABACinematicCameraCut::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}
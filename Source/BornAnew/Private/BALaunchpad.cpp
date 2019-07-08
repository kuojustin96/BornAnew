// Fill out your copyright notice in the Description page of Project Settings.


#include "BALaunchpad.h"
#include "BAPlayerCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ABALaunchpad::ABALaunchpad()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	HitboxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("HitboxComp"));
	HitboxComp->OnComponentBeginOverlap.AddDynamic(this, &ABALaunchpad::OnHitboxBeginOverlap);
	HitboxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitboxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	HitboxComp->SetupAttachment(MeshComp);

	ForceAmount = 2000.0f;
}


void ABALaunchpad::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABAPlayerCharacter* PlayerCharacter = Cast<ABAPlayerCharacter>(OtherActor);
	if (PlayerCharacter != nullptr)
	{
		FVector Force = FVector(0, 0, ForceAmount);
		PlayerCharacter->ApplyForce(Force, false, true);
	}
}
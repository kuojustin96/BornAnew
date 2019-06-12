// Fill out your copyright notice in the Description page of Project Settings.


#include "BAMovingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

// Sets default values
ABAMovingPlatform::ABAMovingPlatform()
{
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	TargetPositionComp = CreateDefaultSubobject<USceneComponent>(TEXT("TargetPosition"));
	TargetPositionComp->SetupAttachment(RootComponent);

	HitboxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("HitboxComp"));
	HitboxComp->SetupAttachment(MeshComp);
	HitboxComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	HitboxComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	HitboxComp->OnComponentBeginOverlap.AddDynamic(this, &ABAMovingPlatform::OnHitboxBeginOverlap);
	HitboxComp->OnComponentEndOverlap.AddDynamic(this, &ABAMovingPlatform::OnHitboxEndOverlap);

	BeginOverlapDelayTime = 1.0f;
	EndOverlapDelayTime = 1.0f;
	MovementSpeedToTarget = 100.0f;
	MovementSpeedToInitialPosition = 100.0f;

	bCanTrigger = true;
	bPlayerGotOff = false;
}


// Called when the game starts or when spawned
void ABAMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	InitalMeshPosition = MeshComp->GetComponentLocation();
}


void ABAMovingPlatform::OnHitboxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCanTrigger == true)
	{
		bCanTrigger = false;

		GetWorldTimerManager().SetTimer(MovementDelayTimerHandle, this, &ABAMovingPlatform::MoveToTargetPosition, 0.01f, true, BeginOverlapDelayTime);
	}
}


void ABAMovingPlatform::OnHitboxEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bCanTrigger == false)
	{
		if (MeshComp->GetComponentLocation().Equals(TargetPositionComp->GetComponentLocation()) && bPlayerGotOff == false)
		{
			GetWorldTimerManager().SetTimer(MovementDelayTimerHandle, this, &ABAMovingPlatform::ReturnToInitialPosition, 0.01f, true, EndOverlapDelayTime);
		}
		else
		{
			bPlayerGotOff = true;
		}
	}
}


void ABAMovingPlatform::MoveToTargetPosition()
{
	//Check if the platform has reached the targegt position
	if (MeshComp->GetComponentLocation().Equals(TargetPositionComp->GetComponentLocation()))
	{
		GetWorldTimerManager().ClearTimer(MovementDelayTimerHandle);

		if (bPlayerGotOff == true)
		{
			GetWorldTimerManager().SetTimer(MovementDelayTimerHandle, this, &ABAMovingPlatform::ReturnToInitialPosition, 0.01f, true, EndOverlapDelayTime);
		}

		return;
	}

	FVector NewPos = FMath::VInterpConstantTo(MeshComp->GetComponentLocation(), TargetPositionComp->GetComponentLocation(), GetWorld()->GetDeltaSeconds(), MovementSpeedToTarget);
	MeshComp->SetWorldLocation(NewPos);
}


void ABAMovingPlatform::ReturnToInitialPosition()
{
	if (MeshComp->GetComponentLocation().Equals(InitalMeshPosition))
	{
		bCanTrigger = true;
		bPlayerGotOff = false;
		GetWorldTimerManager().ClearTimer(MovementDelayTimerHandle);

		return;
	}

	FVector NewPos = FMath::VInterpConstantTo(MeshComp->GetComponentLocation(), InitalMeshPosition, GetWorld()->GetDeltaSeconds(), MovementSpeedToInitialPosition);
	MeshComp->SetWorldLocation(NewPos);
}
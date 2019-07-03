// Fill out your copyright notice in the Description page of Project Settings.


#include "BAContinuousMovingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
ABAContinuousMovingPlatform::ABAContinuousMovingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	RootComponent = RootComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	WaypointPosition = CreateDefaultSubobject<USceneComponent>(TEXT("WaypointPosition"));
	WaypointPosition->SetupAttachment(RootComponent);

	PlatformSpeed = 500.0f;
	TimeAtWaypoint = 0.0f;
	bCanMove = true;
}

// Called when the game starts or when spawned
void ABAContinuousMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	OrigPosition = MeshComp->GetComponentLocation();
	TargetPosition = WaypointPosition->GetComponentLocation();
}

// Called every frame
void ABAContinuousMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanMove == false)
	{
		return;
	}

	FVector NewPos = FMath::VInterpConstantTo(MeshComp->GetComponentLocation(), TargetPosition, DeltaTime, PlatformSpeed);
	MeshComp->SetWorldLocation(NewPos);

	if (MeshComp->GetComponentLocation().Equals(TargetPosition))
	{
		if (MeshComp->GetComponentLocation().Equals(OrigPosition))
		{
			TargetPosition = WaypointPosition->GetComponentLocation();
		}
		else
		{
			TargetPosition = OrigPosition;
		}

		if (TimeAtWaypoint > 0.0f)
		{
			bCanMove = false;

			FTimerHandle MovementTimerHandle;
			GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &ABAContinuousMovingPlatform::EnableMovement, TimeAtWaypoint, false);
		}
	}
}


void ABAContinuousMovingPlatform::EnableMovement()
{
	bCanMove = true;
}
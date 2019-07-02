// Fill out your copyright notice in the Description page of Project Settings.


#include "BASpinningPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
ABASpinningPlatform::ABASpinningPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	bIsConstantlySpinning = false;
	bPingPong = false;
	bPing = true;
	bCanSpin = true;
	TimeBetweenSpins = 1.0f;
	SpinSpeed = 5.0f;
	SpinTargetAmount = 90.0f;
	PingPongSpinAmount = SpinSpeed;
	CurrentSpinAmount = 0.0f;
}

// Called when the game starts or when spawned
void ABASpinningPlatform::BeginPlay()
{
	Super::BeginPlay();

	OrigRotation = MeshComp->GetComponentRotation();
	TargetSpinAmount = FMath::Abs(OrigRotation.Yaw - SpinTargetAmount);
	TargetRotation = FRotator(OrigRotation.Pitch,  OrigRotation.Yaw + SpinTargetAmount, OrigRotation.Roll);
}


// Called every frame
void ABASpinningPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanSpin == false)
	{
		return;
	}

	if (bIsConstantlySpinning == true)
	{
		MeshComp->AddLocalRotation(FRotator(0.0f, SpinSpeed, 0.0f));
	}
	else if (bPingPong == true)
	{
		if (bPing == true)
		{
			MeshComp->AddLocalRotation(FRotator(0.0f, SpinSpeed, 0.0f));
		}
		else
		{
			MeshComp->AddLocalRotation(FRotator(0.0f, -SpinSpeed, 0.0f));
		}

		CurrentSpinAmount += SpinSpeed;
		if (CurrentSpinAmount >= TargetSpinAmount)
		{
			bCanSpin = false;

			MeshComp->SetRelativeRotation(TargetRotation);
			CurrentSpinAmount = 0.0f;

			if (bPing == true)
			{
				TargetRotation = OrigRotation;
				bPing = false;
			}
			else
			{
				TargetRotation = FRotator(OrigRotation.Pitch, OrigRotation.Yaw + SpinTargetAmount, OrigRotation.Roll);
				bPing = true;
			}

			FTimerHandle EnableSpinTimerHandle;
			GetWorldTimerManager().SetTimer(EnableSpinTimerHandle, this, &ABASpinningPlatform::EnableSpin, TimeBetweenSpins, false);
		}
	}
	else
	{
		MeshComp->AddLocalRotation(FRotator(0.0f, SpinSpeed, 0.0f));
		CurrentSpinAmount += SpinSpeed;
		if (CurrentSpinAmount >= TargetSpinAmount)
		{
			bCanSpin = false;

			MeshComp->SetRelativeRotation(TargetRotation);
			CurrentSpinAmount = 0.0f;

			TargetRotation = FRotator(MeshComp->GetComponentRotation().Pitch, MeshComp->GetComponentRotation().Yaw + SpinTargetAmount, MeshComp->GetComponentRotation().Roll);

			FTimerHandle EnableSpinTimerHandle;
			GetWorldTimerManager().SetTimer(EnableSpinTimerHandle, this, &ABASpinningPlatform::EnableSpin, TimeBetweenSpins, false);
		}
	}
}


void ABASpinningPlatform::EnableSpin()
{
	bCanSpin = true;
}
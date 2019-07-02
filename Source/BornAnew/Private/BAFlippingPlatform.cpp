// Fill out your copyright notice in the Description page of Project Settings.


#include "BAFlippingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
ABAFlippingPlatform::ABAFlippingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	bFlipOnZ = true;
	bIsConstantlyFlipping = false;
	bPingPong = false;
	bPing = true;
	bCanFlip = true;
	TimeBetweenFlips = 1.0f;
	FlipSpeed = 5.0f;
	FlipTargetAmount = 90.0f;
	PingPongFlipAmount = FlipSpeed;
	CurrentFlipAmount = 0.0f;
}

// Called when the game starts or when spawned
void ABAFlippingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	OrigRotation = MeshComp->GetComponentRotation();

	if (bFlipOnZ == true)
	{
		TargetFlipAmount = FMath::Abs(OrigRotation.Roll - FlipTargetAmount);
		TargetRotation = FRotator(OrigRotation.Pitch, OrigRotation.Yaw, OrigRotation.Roll + FlipTargetAmount);
	}
	else
	{
		TargetFlipAmount = FMath::Abs(OrigRotation.Pitch - FlipTargetAmount);
		TargetRotation = FRotator(OrigRotation.Pitch + FlipTargetAmount, OrigRotation.Yaw, OrigRotation.Roll);
	}
}


// Called every frame
void ABAFlippingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanFlip == false)
	{
		return;
	}

	if (bIsConstantlyFlipping == true)
	{
		if (bFlipOnZ == true)
		{
			MeshComp->AddLocalRotation(FRotator(0.0f, 0.0f, FlipSpeed));
		}
		else
		{
			MeshComp->AddLocalRotation(FRotator(FlipSpeed, 0.0f, 0.0f));
		}
	}
	else if (bPingPong == true)
	{
		if (bFlipOnZ == true)
		{
			if (bPing == true)
			{
				MeshComp->AddLocalRotation(FRotator(0.0f, 0.0f, FlipSpeed));
			}
			else
			{
				MeshComp->AddLocalRotation(FRotator(0.0f, 0.0f, -FlipSpeed));
			}

			CurrentFlipAmount += FlipSpeed;
			if (CurrentFlipAmount >= TargetFlipAmount)
			{
				bCanFlip = false;

				MeshComp->SetRelativeRotation(TargetRotation);
				CurrentFlipAmount = 0.0f;

				if (bPing == true)
				{
					TargetRotation = OrigRotation;
					bPing = false;
				}
				else
				{
					TargetRotation = FRotator(OrigRotation.Pitch, OrigRotation.Yaw, OrigRotation.Roll + FlipTargetAmount);
					bPing = true;
				}

				FTimerHandle EnableFlipTimerHandle;
				GetWorldTimerManager().SetTimer(EnableFlipTimerHandle, this, &ABAFlippingPlatform::EnableMovement, TimeBetweenFlips, false);
			}
		}
		else
		{
			if (bPing == true)
			{
				MeshComp->AddLocalRotation(FRotator(FlipSpeed, 0.0f, 0.0f));
			}
			else
			{
				MeshComp->AddLocalRotation(FRotator(-FlipSpeed, 0.0f, 0.0f));
			}

			CurrentFlipAmount += FlipSpeed;
			if (CurrentFlipAmount >= TargetFlipAmount)
			{
				bCanFlip = false;

				MeshComp->SetRelativeRotation(TargetRotation);
				CurrentFlipAmount = 0.0f;

				if (bPing == true)
				{
					TargetRotation = OrigRotation;
					bPing = false;
				}
				else
				{
					TargetRotation = FRotator(OrigRotation.Pitch + FlipTargetAmount, OrigRotation.Yaw, OrigRotation.Roll);
					bPing = true;
				}

				FTimerHandle EnableFlipTimerHandle;
				GetWorldTimerManager().SetTimer(EnableFlipTimerHandle, this, &ABAFlippingPlatform::EnableMovement, TimeBetweenFlips, false);
			}
		}
	}
	else
	{
		if (bFlipOnZ == true)
		{
			MeshComp->AddLocalRotation(FRotator(0.0f, 0.0f, FlipSpeed));
			CurrentFlipAmount += FlipSpeed;
			if (CurrentFlipAmount >= TargetFlipAmount)
			{
				bCanFlip = false;

				MeshComp->SetRelativeRotation(TargetRotation);
				CurrentFlipAmount = 0.0f;

				TargetRotation = FRotator(MeshComp->GetComponentRotation().Pitch, MeshComp->GetComponentRotation().Yaw, MeshComp->GetComponentRotation().Roll + FlipTargetAmount);

				FTimerHandle EnableFlipTimerHandle;
				GetWorldTimerManager().SetTimer(EnableFlipTimerHandle, this, &ABAFlippingPlatform::EnableMovement, TimeBetweenFlips, false);
			}
		}
		else
		{
			MeshComp->AddLocalRotation(FRotator(FlipSpeed, 0.0f, 0.0f));
			CurrentFlipAmount += FlipSpeed;
			if (CurrentFlipAmount >= TargetFlipAmount)
			{
				bCanFlip = false;

				MeshComp->SetRelativeRotation(TargetRotation);
				CurrentFlipAmount = 0.0f;

				TargetRotation = FRotator(MeshComp->GetComponentRotation().Pitch + FlipTargetAmount, MeshComp->GetComponentRotation().Yaw, MeshComp->GetComponentRotation().Roll);

				FTimerHandle EnableFlipTimerHandle;
				GetWorldTimerManager().SetTimer(EnableFlipTimerHandle, this, &ABAFlippingPlatform::EnableMovement, TimeBetweenFlips, false);
			}
		}
	}
}


void ABAFlippingPlatform::EnableMovement()
{
	bCanFlip = true;
}
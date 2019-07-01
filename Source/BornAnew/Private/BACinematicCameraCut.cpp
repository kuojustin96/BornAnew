// Fill out your copyright notice in the Description page of Project Settings.


#include "BACinematicCameraCut.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "BAPlayerCharacter.h"
#include "TimerManager.h"
#include "BASimpleDialogueDataAsset.h"
#include "BADialogueWidget.h"

// Sets default values
ABACinematicCameraCut::ABACinematicCameraCut()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABACinematicCameraCut::OnTriggerBeginOverlap);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	RootComponent = TriggerBox;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);

	bCanTriggerMultipleTimes = false;
	bHasBeenTriggered = false;

	CameraBlendTODuration = 1.0f;
	CameraBlendFROMDuration = 1.0f;
	CutDuration = 5.0f;
}

// Called when the game starts or when spawned
void ABACinematicCameraCut::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerCharacter = Cast<ABAPlayerCharacter>(PlayerController->GetCharacter());

	if (DialogueDataAsset != nullptr)
	{
		bHasDialogue = true;
	}
	else
	{
		bHasDialogue = false;
	}
}


void ABACinematicCameraCut::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasBeenTriggered == false)
	{
		if (PlayerController != nullptr && PlayerCharacter != nullptr)
		{
			bHasBeenTriggered = true;

			PlayerController->SetViewTargetWithBlend(this, CameraBlendTODuration);
			PlayerCharacter->EnableMovementInputOnPlayer(false);
			PlayerCharacter->DisableMainGameplayUI();

			FTimerHandle CutDurationTimerHandle;
			if (DialogueDataAsset != nullptr)
			{
				//Create a dialogue widget for the player to interact with
				GetWorldTimerManager().SetTimer(CutDurationTimerHandle, this, &ABACinematicCameraCut::CreateDialogueWidget,CameraBlendTODuration, false);
			}
			else
			{
				//Blend the camera back to the default player camera position
				GetWorldTimerManager().SetTimer(CutDurationTimerHandle, this, &ABACinematicCameraCut::BlendToPlayerCam, CutDuration + CameraBlendTODuration, false);
			}
		}
	}
}


void ABACinematicCameraCut::BlendToPlayerCam()
{
	//Blend the camera back to the default player camera position
	if (PlayerController != nullptr && PlayerCharacter != nullptr)
	{
		PlayerController->SetViewTargetWithBlend(PlayerCharacter, CameraBlendFROMDuration);

		FTimerHandle BlendDurationTimerHandle;
		GetWorldTimerManager().SetTimer(BlendDurationTimerHandle, this, &ABACinematicCameraCut::ReEnablePlayerMovementInput, CameraBlendFROMDuration, false);
	}
}


//TODO: Hide main gameplay widget when you enable the dialogue widget
void ABACinematicCameraCut::CreateDialogueWidget()
{
	if (CinematicUIWidget != nullptr)
	{
		UBADialogueWidget* DialogueWidget = CreateWidget<UBADialogueWidget>(GetWorld(), CinematicUIWidget);
		if (DialogueWidget != nullptr && PlayerCharacter != nullptr)
		{
			DialogueWidget->SetRenderOpacity(0.0f);
			PlayerCharacter->EnableInteract(true);

			DialogueWidget->EnableDialogueUI(this, DialogueDataAsset, bCanTriggerMultipleTimes);
			DialogueWidget->AddToViewport();
		}
	}
}


void ABACinematicCameraCut::ReEnablePlayerMovementInput()
{
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->EnableMovementInputOnPlayer(true);
		PlayerCharacter->EnableMainGameplayUI();

		if (bCanTriggerMultipleTimes == true)
		{
			bHasBeenTriggered = false;
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BADialogueWidget.h"
#include "Kismet/GameplayStatics.h"
#include "BASimpleDialogueDataAsset.h"
#include "BAPlayerCharacter.h"
#include "Components/RichTextBlock.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "BACinematicCameraCut.h"


void UBADialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ABAPlayerCharacter* PlayerChar = Cast<ABAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerChar != nullptr)
	{
		//probably want to add a bool in playercharacter that allows for player interaction
		PlayerChar->Interact.AddDynamic(this, &UBADialogueWidget::OnInteract);
	}

	bHasMoreDialogue = true;
	bSetFullDialogue = false;
	SubstringLength = 0;
	CurrentDialogueArrayInt = 0;
}


void UBADialogueWidget::EnableDialogueUI(ABACinematicCameraCut* CinCamCut, UBASimpleDialogueDataAsset* DialogueDataAsset, bool bCanTriggerMultipleTimes)
{
	DialogueData = DialogueDataAsset;
	CinematicCamCut = CinCamCut;
	TargetDialogue = DialogueDataAsset->Dialogue[CurrentDialogueArrayInt].ToString();
	bCanTriggerMultiple = bCanTriggerMultipleTimes;

	//Need to wait a frame so the blueprint code that sets widget variables is run first
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("SetupWidgetComponents"));
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDel);
}


void UBADialogueWidget::SetupWidgetComponents()
{
	if (RichTextBlock != nullptr)
	{
		RichTextBlock->SetText(FText::FromString(FString("")));
	}

	if (InteractButtonImage != nullptr)
	{
		InteractButtonImage->SetRenderOpacity(0.0f);
	}

	SetRenderOpacity(1.0f);

	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("SetDialogue"));
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDel);
}


void UBADialogueWidget::SetDialogue()
{
	if (bHasMoreDialogue == true)
	{
		//Check if there's an image
		if (TargetDialogue.Mid(SubstringLength, 1).Equals(FString("<")))
		{
			int EndOfImage = TargetDialogue.Find("</>", ESearchCase::IgnoreCase, ESearchDir::FromStart, SubstringLength);

			if (EndOfImage > -1)
			{
				//Move SubstringLength pointer to the end of the image
				SubstringLength = EndOfImage + 3;
			}
		}

		FText DialogueText = FText::FromString(TargetDialogue.Left(SubstringLength));
		RichTextBlock->SetText(DialogueText);

		SubstringLength++;

		if (SubstringLength <= TargetDialogue.Len())
		{
			FTimerDelegate TimerDel;
			TimerDel.BindUFunction(this, FName("SetDialogue"));
			GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDel);
		}
		else
		{
			InteractButtonImage->SetRenderOpacity(1.0f);
			bHasMoreDialogue = false;
		}
	}
	else
	{
		RichTextBlock->SetText(DialogueData->Dialogue[CurrentDialogueArrayInt]);
		InteractButtonImage->SetRenderOpacity(1.0f);
	}
}


void UBADialogueWidget::OnInteract()
{
	if (bHasMoreDialogue == true)
	{
		bHasMoreDialogue = false;
	}
	else
	{
		RichTextBlock->SetText(FText::FromString(FString("")));
		InteractButtonImage->SetRenderOpacity(0.0f);

		CurrentDialogueArrayInt++;
		if (CurrentDialogueArrayInt < DialogueData->Dialogue.Num())
		{
			bHasMoreDialogue = true;

			SubstringLength = 0;
			TargetDialogue = DialogueData->Dialogue[CurrentDialogueArrayInt].ToString();

			SetDialogue();
		}
		else
		{
			DisableDialogueUI();
		}
	}
}


void UBADialogueWidget::DisableDialogueUI()
{
	RemoveFromParent();

	CinematicCamCut->BlendToPlayerCam();

	if (bCanTriggerMultiple == false)
	{
		ABAPlayerCharacter* PlayerChar = Cast<ABAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
		if (PlayerChar != nullptr)
		{
			PlayerChar->Interact.RemoveDynamic(this, &UBADialogueWidget::OnInteract);
		}
	}
	else
	{
		bHasMoreDialogue = true;
		bSetFullDialogue = false;
		SubstringLength = 0;
		CurrentDialogueArrayInt = 0;
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BAPlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "BAPlayerAnimInstance.h"
#include "BACollectableObject.h"
#include "Curves/CurveFloat.h"
#include "TimerManager.h"
#include "BAMainGameplayUI.h"


// Sets default values
ABAPlayerCharacter::ABAPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


	CapsuleOverlapComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleOverlapComp"));
	CapsuleOverlapComp->InitCapsuleSize(50.0f, 100.0f);
	CapsuleOverlapComp->SetupAttachment(RootComponent);
	CapsuleOverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ABAPlayerCharacter::OnCapsuleBeginOverlap);

	SphereOverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereOverlapComp"));
	SphereOverlapComp->InitSphereRadius(100.0f);
	SphereOverlapComp->SetupAttachment(RootComponent);
	SphereOverlapComp->OnComponentBeginOverlap.AddDynamic(this, &ABAPlayerCharacter::OnSphereBeginOverlap);

	NumJumps = 0;
	DoubleJumpZVelocity = 600.0f;
	WallJumpZVelocity = 600.0f;
	SlideJumpZDamping = 2.0f;
	
	bInputEnabled = true;
	bInteractEnabled = false;
	bHoldingDownSprintKey = true;
	bIsSliding = false;
	bCanSlide = true;
	bIsOnWall = false;
	bCanWallJump = true;
	
	SprintSpeed = 1000.0f;
	SprintingFOV = 100.0f;
	WallGrabDuration = 1.0f;
	SlideDownWallGravityScale = 0.25f;

	SlideCooldown = 1.0f;
	SlideSlopeThreshold = 0.04f;
	SlideDownSlopeInterpSpeed = 5.0f;
	JumpSlideComboBuffer = 0.25f;
	JumpSlideTraceLength = 200.0f;
	MaxSlideCurveValue = 1.5f;
	TimeSlidingMuliplier = 10.0f;
}

// Called when the game starts or when spawned
void ABAPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnReachedJumpApex.AddDynamic(this, &ABAPlayerCharacter::EnableFallingTrace);

	// Set base values
	BaseJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	BaseWalkSpeed = GetCharacterMovement()->GetMaxSpeed();
	BaseWalkingFOV = FollowCamera->FieldOfView;
	BaseGravityScale = GetCharacterMovement()->GravityScale;
	BaseMaxNumJumps = JumpMaxCount;

	if (BaseSlideSpeedCurve != nullptr)
	{
		MaxHeightToGainSpeed = BaseSlideSpeedCurve->GetFloatValue(1.0f);
	}

	if (GetMesh() != nullptr)
	{
		AnimInstance = Cast<UBAPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	}

	if (MainGameplayUIWidget != nullptr)
	{
		MainGameplayUI = CreateWidget<UBAMainGameplayUI>(GetWorld(), MainGameplayUIWidget);
		MainGameplayUI->AddToViewport();
	}
}


// Called to bind functionality to input
void ABAPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABAPlayerCharacter::OnJump);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ABAPlayerCharacter::OnSprintStart);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ABAPlayerCharacter::OnSprintEnd);
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &ABAPlayerCharacter::OnSlideStart);
	PlayerInputComponent->BindAction("Slide", IE_Released, this, &ABAPlayerCharacter::OnSlideEnd);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABAPlayerCharacter::OnInteract);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABAPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABAPlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABAPlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABAPlayerCharacter::LookUpAtRate);
}


void ABAPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


void ABAPlayerCharacter::EnableMovementInputOnPlayer(bool bEnableInput)
{
	bInputEnabled = bEnableInput;
}


void ABAPlayerCharacter::EnableInteract(bool bEnableInteract)
{
	bInteractEnabled = bEnableInteract;
}


void ABAPlayerCharacter::OnInteract()
{
	if (bInteractEnabled == true)
	{
		Interact.Broadcast();
	}
}


void ABAPlayerCharacter::MoveForward(float Value)
{
	//Check if input is enabled
	if (bInputEnabled == false)
	{
		return;
	}

	if (bIsOnWall)
	{
		return;
	}

	if ((Controller != nullptr) && (Value != 0.0f) && (bIsSliding == false))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}


void ABAPlayerCharacter::MoveRight(float Value)
{
	//Check if input is enabled
	if (bInputEnabled == false)
	{
		return;
	}

	if (bIsOnWall)
	{
		return;
	}

	if ((Controller != nullptr) && (Value != 0.0f) && (bIsSliding == false))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


void ABAPlayerCharacter::TurnAtRate(float Rate)
{
	//Check if input is enabled
	if (bInputEnabled == false)
	{
		return;
	}

	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ABAPlayerCharacter::LookUpAtRate(float Rate)
{
	//Check if input is enabled
	if (bInputEnabled == false)
	{
		return;
	}

	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ABAPlayerCharacter::OnSprintStart()
{
	//Check if not jumping
	if (NumJumps > 0)
	{
		bHoldingDownSprintKey = true;
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	FollowCamera->SetFieldOfView(SprintingFOV);
}


void ABAPlayerCharacter::OnSprintEnd()
{
	bHoldingDownSprintKey = false;
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	FollowCamera->SetFieldOfView(BaseWalkingFOV);
}


void ABAPlayerCharacter::OnJump()
{
	//Check if input is enabled
	if (bInputEnabled == false)
	{
		return;
	}

	GetCharacterMovement()->bNotifyApex = true;
	if (GetWorldTimerManager().IsTimerActive(JumpSlideBufferTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(JumpSlideBufferTimerHandle);
	}

	if (NumJumps > 0)
	{
		GetCharacterMovement()->JumpZVelocity = DoubleJumpZVelocity;
	}

	if (bIsOnWall == true)
	{
		GetCharacterMovement()->JumpZVelocity = WallJumpZVelocity;

		bIsOnWall = false;
		GetCharacterMovement()->GravityScale = BaseGravityScale;
		GetWorldTimerManager().ClearTimer(SlideDownWallTimerHandle);
	}

	if (bIsSliding == true)
	{
		bIsSliding = false;
		GetCharacterMovement()->Velocity = GetCharacterMovement()->Velocity / SlideJumpZDamping;
		UE_LOG(LogTemp, Warning, TEXT("HIT THAT JUMP WITH %f"), GetCharacterMovement()->JumpZVelocity);
	}

	NumJumps++;
	Jump();

	bCanSlide = false;
	
	if (AnimInstance != nullptr)
	{
		AnimInstance->bIsJumping = true;

		//Probably want to set somewhere else
		AnimInstance->bIsSliding = false;
	}
}


void ABAPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (bIsOnWall == true)
	{
		bIsOnWall = false;
		GetCharacterMovement()->GravityScale = BaseGravityScale;
	}

	StopJumping();
	NumJumps = 0;
	GetCharacterMovement()->JumpZVelocity = BaseJumpZVelocity;

	bCanSlide = true;

	//Reset max jumps if the player just wall jumped
	if (JumpMaxCount > BaseMaxNumJumps)
	{
		JumpMaxCount = BaseMaxNumJumps;
		bCanWallJump = true;
	}

	if (bHoldingDownSprintKey == true)
	{
		OnSprintStart();
	}

	if (AnimInstance != nullptr)
	{
		AnimInstance->bIsJumping = false;
		AnimInstance->bIsFalling = false;
	}
}


void ABAPlayerCharacter::EnableFallingTrace()
{
	GetWorldTimerManager().SetTimer(JumpSlideBufferTimerHandle, this, &ABAPlayerCharacter::CheckForJumpSlideCombo, 0.01f, true);
}


void ABAPlayerCharacter::CheckForJumpSlideCombo()
{
	FHitResult OutHit;
	FVector End = (-GetActorUpVector() * JumpSlideTraceLength) + GetActorLocation();
	FCollisionQueryParams CollisionParams;

	//DrawDebugLine(GetWorld(), GetActorLocation(), End, FColor::Green, false, 0, 0, 5);

	if (GetWorld()->LineTraceSingleByChannel(OutHit, GetActorLocation(), End, ECC_Visibility, CollisionParams))
	{
		if (OutHit.bBlockingHit == true)
		{
			GetWorldTimerManager().ClearTimer(JumpSlideBufferTimerHandle);

			EnableSliding();
		}
	}
}


float ABAPlayerCharacter::GetCurrentSlopeAngle()
{
	FHitResult OutHit;
	FVector End = (-GetActorUpVector() * 150.0f) + GetActorLocation();
	FCollisionQueryParams CollisionParams;

	float SlopeAngle = 0.0f;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, GetActorLocation(), End, ECC_Visibility, CollisionParams))
	{
		if (OutHit.bBlockingHit == true)
		{
			SlopeDirection = OutHit.ImpactNormal;

			float DotProductNormal = FVector::DotProduct(GetActorUpVector().GetSafeNormal(), OutHit.ImpactNormal.GetSafeNormal());
			SlopeAngle = FMath::Acos(DotProductNormal);
		}
	}
	else
	{
		//Disallow sliding if the trace doesn't hit anything
		bCanSlide = false;

		UE_LOG(LogTemp, Warning, TEXT("Disable Sliding"));
	}

	return SlopeAngle;
}


void ABAPlayerCharacter::OnSlideStart()
{
	//Check if input is enabled
	if (bInputEnabled == false)
	{
		return;
	}

	float SlopeAngle = GetCurrentSlopeAngle();

	//Check if sliding is enabled
	if (bCanSlide == false)
	{
		return;
	}

	//Check if not jumping, override if the player is within the jumpslide combo window
	if (NumJumps > 0 && bCanSlide == false)
	{
		return;
	}

	bIsSliding = true;
	bCanSlide = false;

	//if player is not a slope, run original code
	//else start a timer that will maintain whatever velocity should be applied until the slide button is released
	//	or until the slope evens out, 

	float CurveValue = GetCharacterMovement()->Velocity.Size() / SprintSpeed;

	//Probably want to change this to a slope angle threshold
	if (SlopeAngle <= SlideSlopeThreshold)
	{
		//Make sure the player isnt over the max speed threshold
		if (GetCharacterMovement()->Velocity.Size() < MaxMovementSpeed && GetCharacterMovement()->Velocity.Size() != 0.0f)
		{
			if (CurveValue < MaxSlideCurveValue)
			{
				float SlideSpeed = BaseSlideSpeedCurve->GetFloatValue(CurveValue);
				GetCharacterMovement()->AddImpulse(GetActorForwardVector() * SlideSpeed * 5.0f, true);
			}
		}
	}
	else
	{
		if (GetCharacterMovement()->Velocity.Size() < MaxMovementSpeed)
		{
			//Need to trace down and get the current slope value to change the velocity
			TimeSliding = 0.0f;
			GetWorldTimerManager().SetTimer(SlidingOnSlopeTimerHandle, this, &ABAPlayerCharacter::MaintainSlidingSpeed, 0.01f, true);
		}
	}

	if (AnimInstance != nullptr)
	{
		AnimInstance->bIsSliding = true;
	}
}


void ABAPlayerCharacter::MaintainSlidingSpeed()
{
	//Check if input is enabled
	if (bInputEnabled == false)
	{
		return;
	}

	float SlideAngle = GetCurrentSlopeAngle();

	if (bIsSliding == false)
	{
		return;
	}

	if (SlideAngle <= SlideSlopeThreshold)
	{
		GetWorldTimerManager().ClearTimer(SlidingOnSlopeTimerHandle);

		return;
	}

	TimeSliding += GetWorldTimerManager().GetTimerElapsed(SlidingOnSlopeTimerHandle);

	float SlideMultiplier =  SlideOnSlopeSpeedCurve->GetFloatValue(SlideAngle + (TimeSliding * TimeSlidingMuliplier));

	FVector SlideSpeed = SlopeDirection * SlideMultiplier * 5.0f;

	if (SlideAngle >= SlideSlopeThreshold)
	{
		float Yaw = FMath::FInterpTo(GetActorRotation().Yaw, SlopeDirection.Rotation().Yaw, GetWorld()->GetDeltaSeconds(), SlideDownSlopeInterpSpeed);
		FQuat NewQuat = FQuat::MakeFromEuler(FVector(0.0f, 0.0f, Yaw));
		SetActorRotation(NewQuat);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Maintaining Slide"));
	GetCharacterMovement()->Velocity = SlideSpeed;
}


void ABAPlayerCharacter::OnSlideEnd()
{
	if (bIsSliding == false)
	{
		return;
	}

	bIsSliding = false;
	GetWorldTimerManager().ClearTimer(SlidingOnSlopeTimerHandle);

	GetWorldTimerManager().SetTimer(AllowSlidingTimerHandle, this, &ABAPlayerCharacter::EnableSliding, SlideCooldown, false);

	if (AnimInstance != nullptr)
	{
		AnimInstance->bIsSliding = false;
	}
}


void ABAPlayerCharacter::EnableSliding()
{
	GetWorldTimerManager().ClearTimer(AllowSlidingTimerHandle);
	bCanSlide = true;
}


void ABAPlayerCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag("WallJump"))
	{
		if (NumJumps > 0 && bCanWallJump == true)
		{
			bIsOnWall = true;
			bCanWallJump = false;

			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->GravityScale = 0.0f;
			JumpMaxCount = BaseMaxNumJumps + 1;

			GetWorldTimerManager().SetTimer(SlideDownWallTimerHandle, this, &ABAPlayerCharacter::SlideDownWall, WallGrabDuration, false);
		}
	}
}


void ABAPlayerCharacter::SlideDownWall()
{
	GetCharacterMovement()->GravityScale = SlideDownWallGravityScale;
	GetWorldTimerManager().ClearTimer(SlideDownWallTimerHandle);
}


void ABAPlayerCharacter::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABACollectableObject* Collectable = Cast<ABACollectableObject>(OtherActor);
	if (Collectable != nullptr)
	{
		Collectable->OnCollected();
	}
}


void ABAPlayerCharacter::EnableMainGameplayUI()
{
	if (MainGameplayUI != nullptr)
	{
		MainGameplayUI->SetRenderOpacity(1.0f);
		MainGameplayUI->SetIsEnabled(true);
	}
}


void ABAPlayerCharacter::DisableMainGameplayUI()
{
	if (MainGameplayUI != nullptr)
	{
		MainGameplayUI->SetRenderOpacity(0.0f);
		MainGameplayUI->SetIsEnabled(false);
	}
}


void ABAPlayerCharacter::ApplyForce(FVector ForceAmount, bool bOverrideXY, bool bOverrideZ, int32 JumpsToAdd /*= 1*/)
{
	LaunchCharacter(ForceAmount, bOverrideXY, bOverrideZ);
	
	NumJumps += JumpsToAdd;

	if (AnimInstance != nullptr)
	{
		AnimInstance->bIsFalling = true;
	}
}
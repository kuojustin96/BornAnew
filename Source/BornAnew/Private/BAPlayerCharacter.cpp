// Fill out your copyright notice in the Description page of Project Settings.


#include "BAPlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "BAPlayerAnimInstance.h"
#include "Curves/CurveFloat.h"
#include "TimerManager.h"

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

	NumJumps = 0;
	DoubleJumpZVelocity = 600.0f;
	WallJumpZVelocity = 600.0f;
	
	bIsSliding = false;
	bCanSlide = true;
	bIsOnWall = false;
	bCanWallJump = true;
	
	SprintSpeed = 1000.0f;
	SprintingFOV = 100.0f;
	WallGrabDuration = 1.0f;
	SlideDownWallGravityScale = 0.25f;

	SlideCooldown = 1.0f;
	JumpSlideComboBuffer = 0.25f;
	JumpSlideTraceLength = 200.0f;
	SlideImpulseBoostTolerance = 1.5f;
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

	if (JumpSlideComboSpeedCurve != nullptr)
	{
		MaxHeightToGainSpeed = JumpSlideComboSpeedCurve->GetFloatValue(1.0f);
	}

	if (GetMesh() != nullptr)
	{
		AnimInstance = Cast<UBAPlayerAnimInstance>(GetMesh()->GetAnimInstance());
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


void ABAPlayerCharacter::MoveForward(float Value)
{
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
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void ABAPlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}


void ABAPlayerCharacter::OnSprintStart()
{
	//Check if not jumping
	if (NumJumps > 0)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	FollowCamera->SetFieldOfView(SprintingFOV);
}


void ABAPlayerCharacter::OnSprintEnd()
{
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	FollowCamera->SetFieldOfView(BaseWalkingFOV);
}


void ABAPlayerCharacter::OnJump()
{
	GetCharacterMovement()->bNotifyApex = true;
	if (GetWorldTimerManager().IsTimerActive(JumpSlideBufferTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(JumpSlideBufferTimerHandle);
	}

	if (NumJumps > 0)
	{
		GetCharacterMovement()->JumpZVelocity = DoubleJumpZVelocity;
	}

	if (bIsOnWall)
	{
		GetCharacterMovement()->JumpZVelocity = WallJumpZVelocity;

		bIsOnWall = false;
		GetCharacterMovement()->GravityScale = BaseGravityScale;
		GetWorldTimerManager().ClearTimer(SlideDownWallTimerHandle);
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

	//Reset max jumps if the player just wall jumped
	if (JumpMaxCount > BaseMaxNumJumps)
	{
		JumpMaxCount = BaseMaxNumJumps;
		bCanWallJump = true;
	}

	if (AnimInstance != nullptr)
	{
		AnimInstance->bIsJumping = false;
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


void ABAPlayerCharacter::OnSlideStart()
{
	if (bCanSlide == false || GetCharacterMovement()->Velocity.Size() == 0.0f)
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

	//Make sure the player isnt over the max speed threshold
	if (GetCharacterMovement()->Velocity.Size() < MaxMovementSpeed) 
	{
		float LookupAmount = GetCharacterMovement()->Velocity.Size() / SprintSpeed;

		if (LookupAmount < SlideImpulseBoostTolerance)
		{
			float SlideSpeed = JumpSlideComboSpeedCurve->GetFloatValue(LookupAmount);
			GetCharacterMovement()->AddImpulse(GetActorForwardVector() * SlideSpeed * 5.0f, true);
		}
	}

	if (AnimInstance != nullptr)
	{
		AnimInstance->bIsSliding = true;
	}
}


void ABAPlayerCharacter::OnSlideEnd()
{
	if (bIsSliding == false)
	{
		return;
	}

	bIsSliding = false;

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
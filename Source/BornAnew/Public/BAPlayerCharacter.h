// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BAPlayerCharacter.generated.h"


class UBAPlayerAnimInstance;


UCLASS()
class BORNANEW_API ABAPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABAPlayerCharacter();

public:	
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	void OnJump();

	virtual void Landed(const FHitResult& Hit) override;

	void OnSprintStart();

	void OnSprintEnd();

	void OnSlideStart();

	void OnSlideEnd();

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SlideDownWall();

	void EnableSliding();

	float GetCurrentSlopeAngle();

	UFUNCTION()
	void MaintainSlidingSpeed();

	UFUNCTION()
	void EnableFallingTrace();

	UFUNCTION()
	void CheckForJumpSlideCombo();

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UBAPlayerAnimInstance* AnimInstance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CapsuleOverlapComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* SphereOverlapComp;

protected:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	int32 NumJumps;

	/** Jump velocity of the 2nd jump */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump")
	float DoubleJumpZVelocity;

	float BaseJumpZVelocity;

	/** Absolute maximum speed a player can move at */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxMovementSpeed;

	/** Max height difference that will be used to calculate the amount of speed gained
	when successfully hitting a jump slide combo. Max hiehgt must be set in the curve.
	HeightDifference = JumpApexZ - LandingZ */
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float MaxHeightToGainSpeed;

	/** Speed that the player will sprint at */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	float BaseWalkSpeed;

	/** FOV the camera will transition to when sprinting */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float SprintingFOV;

	float BaseWalkingFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
	class UCurveFloat* BaseSlideSpeedCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Sliding")
	class UCurveFloat* SlideOnSlopeSpeedCurve;

	/** Minimum angle the slope must be before the  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sliding")
	float SlideSlopeThreshold;

	UPROPERTY(BlueprintReadOnly, Category = "Sliding")
	bool bIsSliding;

	FVector SlopeDirection;

	float TimeSliding;

	/** Multiplier amount that will effect how quickly the player gains speed when sliding down a slope */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sliding")
	float TimeSlidingMuliplier;

	/** Max value on the sliding curve that will be allowed */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sliding")
	float MaxSlideCurveValue;

	bool bCanSlide;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sliding")
	float SlideCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sliding")
	float SlideDownSlopeInterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sliding")
	float JumpSlideComboBuffer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sliding")
	float JumpSlideTraceLength;

	UPROPERTY(BlueprintReadOnly, Category = "Wall Jump")
	bool bIsOnWall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Jump")
	float WallJumpZVelocity;

	bool bCanWallJump;

	float BaseGravityScale;

	int32 BaseMaxNumJumps;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Jump")
	float WallGrabDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Jump")
	float SlideDownWallGravityScale;

protected:
	FTimerHandle AllowSlidingTimerHandle;

	FTimerHandle SlideDownWallTimerHandle;

	FTimerHandle JumpSlideBufferTimerHandle;

	FTimerHandle SlidingOnSlopeTimerHandle;
};

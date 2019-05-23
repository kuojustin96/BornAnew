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

protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FollowCamera;

	UBAPlayerAnimInstance* AnimInstance;

	UCapsuleComponent* CapsuleOverlapComp;

protected:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	int32 NumJumps;

	/** Jump velocity of the 2nd jump */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float DoubleJumpZVelocity;

	float BaseJumpZVelocity;

	/** Speed that the player will sprint at */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;

	float BaseWalkSpeed;

	/** FOV the camera will transition to when sprinting */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintingFOV;

	float BaseWalkingFOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSliding;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsOnWall;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WallJumpZVelocity;

	bool bCanWallJump;

	float BaseGravityScale;

	int32 BaseMaxNumJumps;

	FTimerHandle SlideDownWallTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float WallGrabDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SlideDownWallGravityScale;
};

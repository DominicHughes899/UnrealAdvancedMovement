// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AdvancedMovementCharacter.generated.h"

UCLASS()
class ADVANCEDMOVEMENT_API AAdvancedMovementCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdvancedMovementCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ==== Basic Movement ====

	APlayerController* PlayerController;

	UCharacterMovementComponent* MovementComponent;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void MovementTick();

	void JumpPressed();

	FVector InputVector = FVector(0.f, 0.f, 0.f);

	bool MovementEnabled = true;


	// ==== Camera ====

	void LookYaw(float Value);
	void LookPitch(float Value);
	void LookTick(float DeltaTime);

	FVector2D LookVector = FVector2D(0.f, 0.f);

	UPROPERTY(EditAnywhere)
	float LookSensitivity = 5.f;
	

	// ==== Sprint ====

	void BeginSprint();
	void EndSprint();
	void SprintTick();
	
	bool IsSprinting = false;
	float WalkSpeed = 300.f;
	float RunSpeed = 650.f;


	// ==== Ledge Detection ====

	void DetectLedge();
	FVector FindLedgeLocation(FHitResult Body);

	void BeginClimbLedge();
	void ClimbLedgeUpdate(float DeltaTime);
	void EndClimbLedge();
	
	bool IsClimbingLedge = false;
	FVector MantleStartLocation;
	FVector MantleEndLocation;

	float MantleTime = .5f;
	float MantleTimer = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool LedgeAvailable;

	UPROPERTY(BlueprintReadOnly)
	FVector LedgeLocation;

	
	// ==== WallRun ====

	bool WallRunTrace();
	void WallRunTick(float DeltaTime);

	void BeginWallRun();
	void EndWallRun();

	FVector LaunchVector;
	FVector WallTangent;

	UPROPERTY(BlueprintReadOnly)
	bool IsWallRunning = false;

	UPROPERTY(BlueprintReadOnly)
	bool CanWallRun = false;

	UPROPERTY(BlueprintReadOnly)
	FVector WallRunLocation;


	// ==== Grapple Hook ====

	void FireGrapple();



	// ==== Blueprint Functions ====

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateText();

};

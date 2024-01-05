// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvancedMovementCharacter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AAdvancedMovementCharacter::AAdvancedMovementCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	JumpMaxCount = 2;
}

// Called when the game starts or when spawned
void AAdvancedMovementCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	MovementComponent = GetCharacterMovement();


	MovementComponent->MaxWalkSpeed = 300.f;
}

// Called every frame
void AAdvancedMovementCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Movement Ticks
	MovementTick();
	LookTick(DeltaTime);
	SprintTick();

	// Ledge Detection ticks
	DetectLedge();
	ClimbLedgeUpdate(DeltaTime);

	// Wall Run
	WallRunTick(DeltaTime);

	// Blueprint functions
	UpdateText();
}

// Called to bind functionality to input
void AAdvancedMovementCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("IAx_MoveForward", this, &AAdvancedMovementCharacter::MoveForward);
	InputComponent->BindAxis("IAx_MoveRight", this, &AAdvancedMovementCharacter::MoveRight);
	InputComponent->BindAxis("IAx_LookHorizontal", this, &AAdvancedMovementCharacter::LookYaw);
	InputComponent->BindAxis("IAx_LookVertical", this, &AAdvancedMovementCharacter::LookPitch);

	InputComponent->BindAction("IA_Sprint", IE_Pressed, this, &AAdvancedMovementCharacter::BeginSprint);
	InputComponent->BindAction("IA_Sprint", IE_Released, this, &AAdvancedMovementCharacter::EndSprint);
	InputComponent->BindAction("IA_Jump", IE_Pressed, this, &AAdvancedMovementCharacter::JumpPressed);
}

void AAdvancedMovementCharacter::MoveForward(float Value)
{
	InputVector.X = Value;
}

void AAdvancedMovementCharacter::MoveRight(float Value)
{
	InputVector.Y = Value;
}

void AAdvancedMovementCharacter::MovementTick()
{
	if (MovementEnabled)
	{
		FVector MovementInput = (GetActorForwardVector() * InputVector.X) + (GetActorRightVector() * InputVector.Y);

		AddMovementInput(MovementInput);
	}
}

void AAdvancedMovementCharacter::LookYaw(float Value)
{
	LookVector.X = Value;
}

void AAdvancedMovementCharacter::LookPitch(float Value)
{
	LookVector.Y = Value;
}

void AAdvancedMovementCharacter::LookTick(float DeltaTime)
{
	float YawValue = LookVector.X * LookSensitivity * DeltaTime;
	float PitchValue = LookVector.Y * LookSensitivity * DeltaTime;

	AddControllerYawInput(YawValue);
	AddControllerPitchInput(PitchValue);
	
}

void AAdvancedMovementCharacter::BeginSprint()
{
	IsSprinting = true;
	MovementComponent->MaxWalkSpeed = RunSpeed;
}

void AAdvancedMovementCharacter::EndSprint()
{
	IsSprinting = false;
	MovementComponent->MaxWalkSpeed = WalkSpeed;
}

void AAdvancedMovementCharacter::SprintTick()
{
	if (IsSprinting)
	{
		if (InputVector.X <= 0.f)
		{
			EndSprint();
		}
	}
}

void AAdvancedMovementCharacter::JumpPressed()
{
	if (LedgeAvailable)
	{
		BeginClimbLedge();
	}
	else if (IsWallRunning)
	{
		LaunchCharacter(LaunchVector * 420.f, false, false);
	}
	else if (CanWallRun)
	{
		BeginWallRun();
	}
	else
	{
		Jump();
		 
		UE_LOG(LogTemp, Warning, TEXT("Jumping..."));
	}
}

void AAdvancedMovementCharacter::DetectLedge()
{
	LedgeAvailable = false;
	if (MovementComponent->IsFalling())
	{
		// Line trace from Torso

		FHitResult hitResultBody;
		FVector startLocationBody = GetActorLocation();
		FVector endLocationBody = startLocationBody + (GetActorForwardVector() * 400);

		// Line trace from Head

		FHitResult hitResultHead;
		FVector startLocationHead = startLocationBody + FVector(0.f, 0.f, 70.f);
		FVector endLocationHead = endLocationBody + FVector(0.f, 0.f, 70.f);


		bool hitSuccessBody = GetWorld()->LineTraceSingleByChannel(hitResultBody,
			startLocationBody,
			endLocationBody,
			ECC_GameTraceChannel1);

		if (hitSuccessBody)
		{	
			// Run head line trace only if torso line trace hits

			bool hitSuccessHead = GetWorld()->LineTraceSingleByChannel(hitResultHead,
				startLocationHead,
				endLocationHead,
				ECC_GameTraceChannel1);

			if (!hitSuccessHead)
			{
				// Run specific ledge location finder only if the head trace fails

				LedgeLocation = FindLedgeLocation(hitResultBody);

				LedgeAvailable = true;
			}
		}
	}

	

}

FVector AAdvancedMovementCharacter::FindLedgeLocation(FHitResult Body)
{
	// Run new trace from above impact point, down to find precise location of ledge

	FVector endLocation = Body.ImpactPoint;
	FVector startLocation = endLocation;
	startLocation.Z += 70.f;

	FHitResult hitResult;

	bool success = GetWorld()->SweepSingleByChannel(hitResult,
		startLocation,
		endLocation,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(5.f));

	if (success)
	{
		DrawDebugSphere(GetWorld(), hitResult.ImpactPoint, 5.f, 32, FColor::Red, false, 5.f);

		return(hitResult.ImpactPoint);
	}

	return FVector();
}

void AAdvancedMovementCharacter::BeginClimbLedge()
{
	// Activate Climb Update
	IsClimbingLedge = true;

	// Set Ledge Location
	
	MantleStartLocation = GetActorLocation();
	MantleEndLocation = LedgeLocation;
	MantleEndLocation += GetActorForwardVector() * 100;
	MantleEndLocation.Z += 100.f;

	// Disable Input
	MovementEnabled = false;

	// Reset Movement 
	MovementComponent->StopMovementImmediately();

}

void AAdvancedMovementCharacter::ClimbLedgeUpdate(float DeltaTime)
{
	if (IsClimbingLedge)
	{
		MantleTimer += DeltaTime;

		float MantleAlpha = MantleTimer / MantleTime;
		FVector NewLoc = FMath::Lerp(MantleStartLocation, MantleEndLocation, MantleAlpha);

		SetActorLocation(NewLoc);
		
		if (MantleTimer >= MantleTime)
		{
			EndClimbLedge();
		}
	}
}

void AAdvancedMovementCharacter::EndClimbLedge()
{
	// Disable climb update
	IsClimbingLedge = false;

	// Reset Timer
	MantleTimer = 0.f;
	
	// Enable Movement
	MovementEnabled = true;

}

bool AAdvancedMovementCharacter::WallRunTrace()
{
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation + (((GetActorForwardVector() * 2.f) + GetActorRightVector()) * 100);

	bool Trace = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_GameTraceChannel2);

	if (Trace)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trace Hit"));

		WallRunLocation = HitResult.ImpactPoint + HitResult.ImpactNormal * 50.f;

		LaunchVector = HitResult.ImpactNormal + FVector(0.f, 0.f, 1.f);

		return true;
	}

	return false;
}

void AAdvancedMovementCharacter::WallRunTick(float DeltaTime)
{
	if (CanWallRun && !MovementComponent->IsFalling())
	{
		CanWallRun = false;
	}

	FVector LateralVelocity = GetVelocity();
	LateralVelocity.Z = 0.f;

	if (JumpCurrentCount != 0 && LateralVelocity.Length() > 400.f || CanWallRun)
	{
		CanWallRun = WallRunTrace();
	}

	if (IsWallRunning && !CanWallRun)
	{
		EndWallRun();
	}


}

void AAdvancedMovementCharacter::BeginWallRun()
{
	IsWallRunning = true;

	// Change Gravity
	MovementComponent->GravityScale = 0.f;

	// Set Z Velocity to 0

	FVector NewVel = MovementComponent->Velocity;
	NewVel.Z = 0.f;
	MovementComponent->Velocity = NewVel;


}

void AAdvancedMovementCharacter::EndWallRun()
{
	IsWallRunning = false;

	// Change Gravity
	MovementComponent->GravityScale = 1.f;
}





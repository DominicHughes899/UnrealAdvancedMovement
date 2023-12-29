// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvancedMovementCharacter.h"
#include "DrawDebugHelpers.h"


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
	
	MovementComponent = GetCharacterMovement();


	MovementComponent->MaxWalkSpeed = 300.f;
}

// Called every frame
void AAdvancedMovementCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementTick();
	LookTick(DeltaTime);
	SprintTick();

	DetectLedge();
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
	FVector MovementInput = (GetActorForwardVector() * InputVector.X) + (GetActorRightVector() * InputVector.Y);

	AddMovementInput(MovementInput);
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
	Jump();

	if (LedgeAvailable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Climbing"));

		FVector climbLocation = LedgeLocation;
		climbLocation.Z += 60.f;

		SetActorLocation(climbLocation);
	}
}

void AAdvancedMovementCharacter::DetectLedge()
{
	LedgeAvailable = false;
	if (MovementComponent->IsFalling())
	{
		FHitResult hitResultBody;
		FVector startLocationBody = GetActorLocation();
		FVector endLocationBody = startLocationBody + (GetActorForwardVector() * 400);

		FHitResult hitResultHead;
		FVector startLocationHead = startLocationBody + FVector(0.f, 0.f, 70.f);
		FVector endLocationHead = endLocationBody + FVector(0.f, 0.f, 70.f);


		bool hitSuccessBody = GetWorld()->LineTraceSingleByChannel(hitResultBody,
			startLocationBody,
			endLocationBody,
			ECC_GameTraceChannel1);

		if (hitSuccessBody)
		{
			//DrawDebugLine(GetWorld(), startLocationBody, endLocationBody, FColor::Red, false, 5.f);
			
			bool hitSuccessHead = GetWorld()->LineTraceSingleByChannel(hitResultHead,
				startLocationHead,
				endLocationHead,
				ECC_GameTraceChannel1);

			if (!hitSuccessHead)
			{
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
		UE_LOG(LogTemp, Warning, TEXT("Location Found"));

		DrawDebugSphere(GetWorld(), hitResult.ImpactPoint, 5.f, 32, FColor::Red, false, 5.f);

		return(hitResult.ImpactPoint);
	}

	return FVector();
}





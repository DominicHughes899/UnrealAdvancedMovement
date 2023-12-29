// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvancedMovementCharacter.h"


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
	InputComponent->BindAction("IA_Jump", IE_Pressed, this, &AAdvancedMovementCharacter::BeginJump);
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

void AAdvancedMovementCharacter::BeginJump()
{
	Jump();

}


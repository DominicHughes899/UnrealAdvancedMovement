// Fill out your copyright notice in the Description page of Project Settings.


#include "AdvancedMovementCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AAdvancedMovementCharacter::AAdvancedMovementCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAdvancedMovementCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAdvancedMovementCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementTick();
}

// Called to bind functionality to input
void AAdvancedMovementCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("IAx_MoveForward", this, &AAdvancedMovementCharacter::MoveForward);
	InputComponent->BindAxis("IAx_MoveRight", this, &AAdvancedMovementCharacter::MoveRight);
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

	GetCharacterMovement()->AddInputVector(MovementInput);
}


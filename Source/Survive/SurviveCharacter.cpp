// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SurviveCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ASurviveCharacter

ASurviveCharacter::ASurviveCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Default item types to check for
	type = 0;
	bleeding.amt = 0;
	beamMeUp.amt = 0;

	// Defautl values for status
	Health = 1.0;
	Ammo = 0;
	Max_Ammo = 0;
	Storage_Ammo = 0;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASurviveCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Food 1", IE_Pressed, this, &ASurviveCharacter::EatFood1);
	PlayerInputComponent->BindAction("Food 2", IE_Pressed, this, &ASurviveCharacter::EatFood2);
	PlayerInputComponent->BindAction("Food", IE_Pressed, this, &ASurviveCharacter::EatFood);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASurviveCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASurviveCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ASurviveCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ASurviveCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASurviveCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASurviveCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ASurviveCharacter::OnResetVR);
}

// The player eats food to restore health
void ASurviveCharacter::EatFood() {
	// check for food to be injected from the inventory
	// check for type
	// Eat food and take one away from inventory

	type = 0;  // reset the type
}

void ASurviveCharacter::EatFood1() {
	type = 1;
	EatFood();
}

void ASurviveCharacter::EatFood2() {
	type = 2;
	EatFood();
}

// The player heals to not bleed out
void ASurviveCharacter::Heal() {
	// check for healing items in the inventory
	// check for type
	// Bandage up to reduce bleeding
	bleeding.CheckForBleeding();

	type = 0;  // reset the type
}

void ASurviveCharacter::Heal1() {
	type = 1;
	Heal();
}

void ASurviveCharacter::Heal2() {
	type = 2;
	Heal();
}

// Apply boost to the player
void ASurviveCharacter::Boost() {
	// check for boost items in the inventory
	// check for type
	// boost away
	Energy = beamMeUp.amt += 0.25;
	beamMeUp.CheckForBoost();
	type = 0;  // reset the type
}

void ASurviveCharacter::Boost1() {
	type = 1;
	Boost();
}

void ASurviveCharacter::Boost2() {
	type = 2;
	Boost();
}

void ASurviveCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ASurviveCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ASurviveCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ASurviveCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ASurviveCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ASurviveCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASurviveCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
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

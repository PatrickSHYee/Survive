// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Survive.h"
#include "SurviveCharacter.generated.h"

UCLASS(config=Game)
class ASurviveCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ASurviveCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	// Player's Status boost
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		float Health;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		float Energy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		int Ammo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		int Max_Ammo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
		int Storage_Ammo; 

protected:
	/** Eat Food to restore health */
	int type;						// I want to be a pointer to an Item, so it can be taken from the inventory

	void EatFood();
	void EatFood1();
	void EatFood2();

	/** Heal the player if the player is bleeding*/
	FBleed bleeding;
	void Heal1();
	void Heal2();
	void Heal();

	/** Booster the player to reduce bleeding and restore health a little*/
	FBooster beamMeUp;
	void Boost();
	void Boost1();
	void Boost2();

	/** Resets HMD orientation in VR. */
	void OnResetVR();

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

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


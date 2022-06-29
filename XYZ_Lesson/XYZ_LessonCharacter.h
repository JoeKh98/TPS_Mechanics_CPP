// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XYZ_LessonCharacter.generated.h"

UCLASS(config=Game)
class AXYZ_LessonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AXYZ_LessonCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;


	UFUNCTION(BlueprintCallable)
	void SetActiveCar(class AWheeledVehicle* ActiveCar_In) { CurrentActiveCar = ActiveCar_In; };

	bool InACar = false;

	//UFUNCTION(BlueprintCallable)
	//void SetActiveChar(class AXYZ_LessonCharacter* ActiveChar) { CurrentCharacter = ActiveChar; };


	/// <summary>
	/// Created Controller
	/// </summary>
	AController* CurrentController;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AXYZ_LessonCharacter* CurrentCharacter = this;

	//UPROPERTY(BlueprintCallable)
	AXYZ_LessonCharacter* GetCurrentCharacter()
	{
		return CurrentCharacter; 
	}

	//class AController* CharController = GetController();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
		TSubclassOf<AActor> ProjectileType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
		FVector FiringOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
		float FireVelocity = 1000.0; //10 m/s




	/** Resets HMD orientation in VR. */
	void OnResetVR();

	virtual void BeginPlay() override;

	void Fire();


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

	void SitInACar();



	UFUNCTION(BlueprintCallable)
	void LeaveACar();

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



private:
	class AWheeledVehicle* CurrentActiveCar;
	
	

};


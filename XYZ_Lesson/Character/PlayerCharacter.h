// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZ_BaseCharacter.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"



/**
 * 
 */
UCLASS(Blueprintable)
class XYZ_LESSON_API APlayerCharacter : public AXYZ_BaseCharacter
{
	GENERATED_BODY()

public:

	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	virtual void OnSprintStart_Implementation() override;
	virtual void OnSprintEnd_Implementation() override;

	virtual void SwimForward(float Value) override;
	virtual void SwimRight(float Value) override;
	virtual void SwimUp(float Value) override;
	
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override; 

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera ")
		class UCameraComponent* CameraComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera ")
		class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera ")
		UCurveFloat* SpringArmCurve;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Camera ")
	UFUNCTION()
		void UpdateSprintTimeline(float Alpha);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera ")
		float  EndLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera ")
		float StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Camera ")
		float Offset = 500.0f;

	virtual void OnStartAimingInternal() override;

	virtual void OnStopAimingInternal() override;

	//virtual void BeginPlay() override; 

private:
	
	

public:
	
	FOnTimelineFloat UpdateFunctionFloat;
	
	FTimeline SprintSpringArm;
};

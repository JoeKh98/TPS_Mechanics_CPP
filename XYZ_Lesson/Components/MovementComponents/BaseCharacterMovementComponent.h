// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../LedgeDetectorComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParams
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;



};


UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"), 
	CMOVE_Mantling UMETA(DisplayName = "Mantling"), 
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),

	CMOVE_Max UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};



/**
 * 
 */
UCLASS()
class XYZ_LESSON_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

		friend class FSavedMove_XYZ;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void PhysicsRotation(float DeltaTime) override;

	bool IsSprinting() { return bIsSprinting; }
	
	bool IsOutOfStamina() const { return bIsOutOfStamina; }

	void SetIsOutOfStamina(bool bIsOutOfStamina_In) { bIsOutOfStamina = bIsOutOfStamina_In; }

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();
	 
	void StartMantle(const FMantlingMovementParams& MantlingParams);
	void EndMantle();
	bool IsMantling() const;

	void AttachToLadder(const class ALadder* Ladder);
	
	
	float GetActorToCurrentLadderProjection(const FVector & Location) const;

	float GetLadderSpeedRatio() const;

	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);

	bool IsOnLadder() const; 

	const class ALadder* GetCurrentLadder();



protected:

	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations);

	void PhysLadder(float DeltaTime, int32 Iterations);

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float SprintSpeed = 1200.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: out of stamina", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float OutOfStaminaSpeed = 300.0f;

	UPROPERTY(Category = "Character movement: swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float SwimmingCapsuleRadius = 60.0f;


	UPROPERTY(Category = "Character movement: swimming", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float SwimmingCapsuleHalfHeight = 60.0f;
	
	UPROPERTY(Category = "Character movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float ClimbingOnLadderMaxSpeed = 200.0f;

	UPROPERTY(Category = "Character movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float ClimbingOnLadderBrakingDeceleration = 2048.0f;

	UPROPERTY(Category = "Character movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float LadderToCharacterOffset = 60.0f;
	
	UPROPERTY(Category = "Character movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MaxLadderTopOffset = 90.0f;

	UPROPERTY(Category = "Character movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float MinLadderBottomOffset = 90.0f;
	
	UPROPERTY(Category = "Character movement: Ladder", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
		float JumpOffFromLadderSpeed = 500.0f;

	

	class AXYZ_BaseCharacter* GetBaseCharacterOwner() const;

private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;

	FMantlingMovementParams CurrentMantlingParams; 
	FTimerHandle MantlingTimer;
	
	const ALadder* CurrentLadder = nullptr;
	
	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;
};


class FSavedMove_XYZ : public FSavedMove_Character
{
	typedef FSavedMove_Character Super; 

public:
	virtual void Clear() override; //Clears flags
	virtual uint8 GetCompressedFlags() const override; 
	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override; 
	virtual void SetMoveFor(ACharacter* InCharacter, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;

private: 
	uint8 bSavedIsSprinting : 1;     /* : 1 - says that we use only one bit from 8 bit variable */
	uint8 bSavedIsMantling : 1;

};

class NetworkPredictionData_Client_Character_XYZ : public FNetworkPredictionData_Client_Character
{
	typedef FNetworkPredictionData_Client_Character Super; 

public: 
	NetworkPredictionData_Client_Character_XYZ(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override; 
	 

};
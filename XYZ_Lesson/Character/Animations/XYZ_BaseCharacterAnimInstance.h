// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "XYZTypes.h"
#include "XYZ_BaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_LESSON_API UXYZ_BaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		float Direction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		bool bIsFalling = false; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		bool bIsCrouching = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsOutOfStamina = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		bool bIsSwimming = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		bool bIsOnLadder = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		float LadderSpeedRatio = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		bool bIsStrafing = false;
	/// <summary>
	/// //
	/// </summary>

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animation")
		FRotator AimRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
		FVector IKRightFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
		FVector IKLeftFootOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Transient, BlueprintReadOnly, Category = "Settings | IK Setting")
		FVector IKPelvisBoneOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation")
		EEquipableItemType CurrentEquippedItemType = EEquipableItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation | Weapon")
		FTransform ForeGripSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation | Weapon")
		bool bIsAiming; 


private:
	TWeakObjectPtr<class AXYZ_BaseCharacter> CachedBaseCharacter; 


};

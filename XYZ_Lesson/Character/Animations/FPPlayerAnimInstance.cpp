// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerAnimInstance.h"
#include "../FPPlayerCharacter.h"
#include "../Controllers/XYZ_PlayerController.h"

void UFPPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AFPPlayerCharacter>(), TEXT("UFPPlayerAnimInstance::NativeBeginPlay() can use UFPPlayerAnimInstance"));
	CachedFirstPersonCharacterOwner = StaticCast<AFPPlayerCharacter*>(TryGetPawnOwner());

}

void UFPPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedFirstPersonCharacterOwner.IsValid())
	{
		return;
	}

	PlayerCameraPitchAngle = CalculateCameraPitchAngle();

}

float UFPPlayerAnimInstance::CalculateCameraPitchAngle() const
{
	float Result = 0.0f;

	AXYZ_PlayerController* Controller = CachedFirstPersonCharacterOwner->GetController<AXYZ_PlayerController>();

	if (IsValid(Controller) && !Controller->GetIgnoreCameraPitch())
	{
		
		Result = Controller->GetControlRotation().Pitch;
	}

	return Result;
}

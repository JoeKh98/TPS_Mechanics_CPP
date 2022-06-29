// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_BaseCharacterAnimInstance.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Components/MovementComponents/BaseCharacterMovementComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UXYZ_BaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AXYZ_BaseCharacter>(), TEXT("UXYZ_BaseCharacterAnimInstance::NativeBeginPlay() UXYZ_BaseCharacterAnimInstance can be used only with AXYZ_BaseCharacter"))

		CachedBaseCharacter = StaticCast<AXYZ_BaseCharacter*>(TryGetPawnOwner());

}

void UXYZ_BaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds); 
	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}

	//Aiming
	bIsAiming = CachedBaseCharacter->IsAiming();

	UBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();

	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();

	bIsSprinting = CharacterMovement->IsSprinting();
	bIsOutOfStamina = CharacterMovement->IsOutOfStamina();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsOnLadder = CharacterMovement->IsOnLadder(); 

	if (bIsOnLadder)
	{
		 LadderSpeedRatio = CharacterMovement->GetLadderSpeedRatio();
	}
	
	bIsStrafing = !CharacterMovement->bOrientRotationToMovement;
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());

	IKRightFootOffset = FVector((CachedBaseCharacter->GetIKRightFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	IKLeftFootOffset = FVector(-(CachedBaseCharacter->GetIKLeftFootOffset() + CachedBaseCharacter->GetIKPelvisOffset()), 0.0f, 0.0f);
	IKPelvisBoneOffset = FVector(0.0f, 0.0f, CachedBaseCharacter->GetIKPelvisOffset());

	AimRotation = CachedBaseCharacter->GetAimOffset();

	const UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItemType = CharacterEquipment->GetCurrentEquippedItemType();

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipment->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		ForeGripSocketTransform = CurrentRangeWeapon->GetForeGripTransform();
	}
}

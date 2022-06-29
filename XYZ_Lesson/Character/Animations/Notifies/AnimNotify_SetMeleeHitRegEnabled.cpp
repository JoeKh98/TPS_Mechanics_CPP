// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SetMeleeHitRegEnabled.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Melee/MeleeWeaponItem.h"

void UAnimNotify_SetMeleeHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

	Super::Notify(MeshComp, Animation);

	AXYZ_BaseCharacter* CharacterOwner = Cast<AXYZ_BaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	AMeleeWeaponItem* MeleeWeapon = CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeapon();
	if (IsValid(MeleeWeapon))
	{
		MeleeWeapon->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}

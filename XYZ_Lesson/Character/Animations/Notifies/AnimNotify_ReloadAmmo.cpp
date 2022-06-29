// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ReloadAmmo.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"


void UAnimNotify_ReloadAmmo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AXYZ_BaseCharacter* CharacterOwner = Cast<AXYZ_BaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return; 
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->ReloadAmmoInCurrentWeapon(NumberOfAmmo, true);

}

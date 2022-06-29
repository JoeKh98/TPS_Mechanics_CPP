// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimNotify_AttachEquippedItem.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"

void UMyAnimNotify_AttachEquippedItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

	Super::Notify(MeshComp, Animation);

	AXYZ_BaseCharacter* CharacterOwner = Cast<AXYZ_BaseCharacter>(MeshComp->GetOwner());

	if (!IsValid(CharacterOwner))
	{
		return;
	}

	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->AttachCurrentItemToEquippedSocket();
	 

}

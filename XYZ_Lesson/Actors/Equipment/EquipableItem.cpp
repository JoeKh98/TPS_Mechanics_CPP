// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipableItem.h"
#include "Character/XYZ_BaseCharacter.h"

AEquipableItem::AEquipableItem()
{
	SetReplicates(true); 
}

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	if (IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<AXYZ_BaseCharacter>(), TEXT("AEquipableItem::SetOwner() only character can be an owner of Equipable Item"));
		CachedCharacterOwner = StaticCast<AXYZ_BaseCharacter*>(GetOwner());
		if (GetLocalRole() == ROLE_Authority)
		{
			SetAutonomousProxy(true); 
		}
	}
	else
	{
		CachedCharacterOwner = nullptr;
	}


}

EEquipableItemType AEquipableItem::GetItemType() const
{
	return ItemType;
}

UAnimMontage* AEquipableItem::GetCharacterEquipAnimMontage() const
{
	return CharacterEquipAnimMontage;
}

FName AEquipableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}

void AEquipableItem::Equip()
{

	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEquipableItem::UnEquip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

FName AEquipableItem::GetDataTableID() const
{
	return DataTableID;
}

bool AEquipableItem::IsSlotCompatable(EEquipmentSlots Slot) const
{
	return CompatableEquipmentSlots.Contains(Slot); 
}

AXYZ_BaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CachedCharacterOwner.IsValid() ? CachedCharacterOwner.Get() : nullptr;
}

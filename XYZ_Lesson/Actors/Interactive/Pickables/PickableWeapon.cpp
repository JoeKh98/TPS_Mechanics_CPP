// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableWeapon.h"
#include "XYZTypes.h"
#include "Engine/DataTable.h"
#include "Inventory/Items/InventoryItem.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Utils/XYZ_DataTableUtils.h"
#include "Inventory/Items/Equipables/WeaponInventoryItem.h"

APickableWeapon::APickableWeapon()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh); 

}

void APickableWeapon::Interact(AXYZ_BaseCharacter* Character)
{
	FWeaponTableRow* WeaponRow = XYZ_DataTableUtils::FindWeaponData(DataTableID); 

	if (WeaponRow)
	{
		TWeakObjectPtr<UWeaponInventoryItem> Weapon = NewObject<UWeaponInventoryItem>(Character); 
		Weapon->Initialize(DataTableID, WeaponRow->WeaponItemDesctiption); 
		Weapon->SetEquipWeaponClass(WeaponRow->EquipableActor); 
		Character->PickupItem(Weapon); 
		Destroy();
	}
}

FName APickableWeapon::GetActionEventName() const
{
	return ActionInteract;
}

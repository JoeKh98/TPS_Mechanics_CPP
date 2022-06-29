// Fill out your copyright notice in the Description page of Project Settings.


#include "PickablePowerups.h"
#include "XYZTypes.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/XYZ_DataTableUtils.h"
#include "Character/XYZ_BaseCharacter.h"

APickablePowerups::APickablePowerups()
{
	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PowerupMesh"));
	SetRootComponent(PowerupMesh); 

}

void APickablePowerups::Interact(AXYZ_BaseCharacter* Character)
{
	FItemTableRow* ItemData = XYZ_DataTableUtils::FindInventoryItemData(GetDataTableID());

	if (ItemData == nullptr)
	{
		return; 
	}

	TWeakObjectPtr<UInventoryItem> Item = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventoryItemClass));
	Item->Initialize(DataTableID, ItemData->InventoryItemDesctiption); 

	const bool bPickedUp = Character->PickupItem(Item); 

	if (bPickedUp)
	{
		Destroy(); 
	}

}

FName APickablePowerups::GetActionEventName() const
{
	return ActionInteract;
}

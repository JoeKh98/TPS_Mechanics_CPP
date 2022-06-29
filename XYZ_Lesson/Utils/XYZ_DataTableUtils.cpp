

#include "XYZ_DataTableUtils.h"
#include "XYZTypes.h"
#include "Engine/DataTable.h"
#include "Inventory/Items/InventoryItem.h"


FWeaponTableRow* XYZ_DataTableUtils::FindWeaponData(const FName WeaponID)
{
	static const FString contextString(TEXT("Find Weapon Data")); 

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/ThirdPersonCPP/Blueprints/Character/Player/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (WeaponDataTable == nullptr)
	{
		return nullptr; 
	}

	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, contextString);

}

FItemTableRow* XYZ_DataTableUtils::FindInventoryItemData(const FName ItemID)
{
	static const FString contextString(TEXT("Find Item Data"));

	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/ThirdPersonCPP/Blueprints/Character/Player/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));

	if (InventoryItemDataTable == nullptr)
	{
		return nullptr;
	}

	return InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, contextString);

}

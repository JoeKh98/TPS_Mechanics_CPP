// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "InventoryItem.generated.h"

class AEquipableItem;
class APickableItem;
class UInventoryItem; 
class AXYZ_BaseCharacter; 



USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
		FText Name; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
		UTexture2D* Icon; 

};

USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
		TSubclassOf<APickableItem> PickableActor; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
		TSubclassOf<AEquipableItem> EquipableActor; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
		FInventoryItemDescription WeaponItemDesctiption; 

};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public: 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
		TSubclassOf<APickableItem> PickableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
		TSubclassOf<UInventoryItem> InventoryItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
		FInventoryItemDescription InventoryItemDesctiption;

};


UCLASS(Blueprintable)
class XYZ_LESSON_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public: 

	void Initialize(FName DataTableID_In, const FInventoryItemDescription& Description_In);

	FName GetDataTableID() const;
	const FInventoryItemDescription& GetDescription() const; 

	virtual bool IsEquipable() const; 
	virtual bool IsConsumable() const; 

	virtual bool Consume(AXYZ_BaseCharacter* ConsumeTarget) PURE_VIRTUAL(UInventoryItem::Consume, return false;);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
		FName DataTableID = NAME_None; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
		FInventoryItemDescription Description; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
		bool bIsEquipable = false; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory item")
		bool bIsConsumable = false; 

private: 
	bool bIsInitialized = false; 


	
};

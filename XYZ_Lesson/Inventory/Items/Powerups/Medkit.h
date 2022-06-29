// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/InventoryItem.h"
#include "Medkit.generated.h"

/**
 * 
 */

class AXYZ_BaseCharacter; 

UCLASS()
class XYZ_LESSON_API UMedkit : public UInventoryItem
{
	GENERATED_BODY()

public: 
	virtual bool Consume(AXYZ_BaseCharacter* ConsumeTarget) override;  
	
protected: 
	UPROPERTY(EditDefaultsOnly, Category = "Medkit")
		float Health = 25.0f; 

};

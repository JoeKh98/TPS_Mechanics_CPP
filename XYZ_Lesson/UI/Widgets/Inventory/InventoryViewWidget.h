// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryViewWidget.generated.h"

/**
 * 
 */

struct FInventorySlot;
class UInventorySlotWidget; 
class UGridPanel;

UCLASS()
class XYZ_LESSON_API UInventoryViewWidget : public UUserWidget
{
	GENERATED_BODY()

public: 
	void InitializeViewWidget(TArray<FInventorySlot>& InventorySlots);

protected:
	UPROPERTY(meta = (BindWidget))
		UGridPanel* GridPanelItemSlot; 

	UPROPERTY(EditDefaultsOnly, Category = "Item Container View Settings")
		TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass; 

	UPROPERTY(EditDefaultsOnly, Category = "Item Container View Settings")
		int32 ColumnCount = 4; 

	void AddItemSlotView(FInventorySlot& SlotToAdd); 

};

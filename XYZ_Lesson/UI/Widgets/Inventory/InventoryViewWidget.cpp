// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryViewWidget.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "InventorySlotWidget.h"
#include "Components/GridPanel.h"

void UInventoryViewWidget::InitializeViewWidget(TArray<FInventorySlot>& InventorySlots)
{
	for (FInventorySlot& Item : InventorySlots)
	{
		AddItemSlotView(Item); 
	}
}

void UInventoryViewWidget::AddItemSlotView(FInventorySlot& SlotToAdd)
{
	checkf(InventorySlotWidgetClass.Get() != nullptr, TEXT("UInventoryViewWidget::AddItemSlotView widget"));

	UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass);
	 
	if (SlotWidget != nullptr)
	{
		SlotWidget->InitializeItemSlot(SlotToAdd);

		const int32 CurrentSlotCount = GridPanelItemSlot->GetChildrenCount();
		const int32 CurrentSlotRow = CurrentSlotCount / ColumnCount; 
		const int32 CurrentSlotColumn = CurrentSlotCount % ColumnCount; 
		GridPanelItemSlot->AddChildToGrid(SlotWidget, CurrentSlotRow, CurrentSlotColumn);

		SlotWidget->UpdateView(); 
	}
	
			


}

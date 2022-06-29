// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySlotWidget.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Inventory/Items/InventoryItem.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Image.h"


void UInventorySlotWidget::InitializeItemSlot(FInventorySlot& InventorySlot)
{
	LinkedSlot = &InventorySlot;

	FInventorySlot::FInventorySlotUpdate OnInventorySlotUpdate; 
	OnInventorySlotUpdate.BindUObject(this, &UInventorySlotWidget::UpdateView); 
	LinkedSlot->BindOnInventorySlotUpdate(OnInventorySlotUpdate); 

}

void UInventorySlotWidget::UpdateView()
{
	if (LinkedSlot == nullptr)
	{	
		ImageItemIcon->SetBrushFromTexture(nullptr); 
		return; 
	}

	if (LinkedSlot->Item.IsValid())
	{
		
		const FInventoryItemDescription& Description = LinkedSlot->Item->GetDescription(); 
		ImageItemIcon->SetBrushFromTexture(Description.Icon); 
	}
	else
	{
		ImageItemIcon->SetBrushFromTexture(nullptr);
	}

}

void UInventorySlotWidget::SetItemIcon(UTexture2D* Icon)
{
	ImageItemIcon->SetBrushFromTexture(Icon);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (LinkedSlot == nullptr)
	{
		return FReply::Handled(); 
	}

	if (!LinkedSlot->Item.IsValid())
	{
		return FReply::Handled(); 
	}

	FKey MouseBtn = InMouseEvent.GetEffectingButton(); 
	if (MouseBtn == EKeys::RightMouseButton)
	{

		TWeakObjectPtr<UInventoryItem> LinkedSlotItem = LinkedSlot->Item; 

		//Outer - owner object for any Uobject (not only actor, actor comp)  
		AXYZ_BaseCharacter* ItemOwner = Cast<AXYZ_BaseCharacter>(LinkedSlotItem->GetOuter()); 

		if (LinkedSlotItem->Consume(ItemOwner))
		{
			LinkedSlot->ClearSlot(); 
		}

		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton); 
	return Reply.NativeReply; 

}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));


	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), GetClass());
	DragWidget->ImageItemIcon->SetBrushFromTexture(LinkedSlot->Item->GetDescription().Icon);

	DragOperation->DefaultDragVisual = DragWidget; 
	DragOperation->Pivot = EDragPivot::MouseDown; 
	DragOperation->Payload = LinkedSlot->Item.Get(); 
	OutOperation = DragOperation; 

	LinkedSlot->ClearSlot(); 

}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (!LinkedSlot->Item.IsValid())
	{
		LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
		LinkedSlot->UpdateSlotState(); 
		return true; 
	}

	return false; 
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

	LinkedSlot->Item = TWeakObjectPtr<UInventoryItem>(Cast<UInventoryItem>(InOperation->Payload));
	LinkedSlot->UpdateSlotState();
}

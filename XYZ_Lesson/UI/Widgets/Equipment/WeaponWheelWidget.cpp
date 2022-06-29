// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWheelWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Inventory/Items/InventoryItem.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Utils/XYZ_DataTableUtils.h"
#include "Blueprint/WidgetTree.h"
#include "XYZTypes.h"


void UWeaponWheelWidget::InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent)
{
	LinkedEquipmentComponent = EquipmentComponent; 
}

void UWeaponWheelWidget::NextSegment()
{
	CurrentSegmentIndex++; 
	if (CurrentSegmentIndex == EquipmentSlotsSegments.Num())
	{
		CurrentSegmentIndex = 0; 
	}
	SelectSegment(); 

}

void UWeaponWheelWidget::PreviousSegment()
{
	CurrentSegmentIndex--;
	if (CurrentSegmentIndex < 0)
	{
		CurrentSegmentIndex = EquipmentSlotsSegments.Num() - 1;
	}
	SelectSegment();
}

void UWeaponWheelWidget::ConfirmSelection()
{
	EEquipmentSlots SelectedSlot = EquipmentSlotsSegments[CurrentSegmentIndex];
	LinkedEquipmentComponent->EquipItemInSlot(SelectedSlot); 
	RemoveFromParent(); 
}

void UWeaponWheelWidget::NativeConstruct()
{
	Super::NativeConstruct(); 
	if (IsValid(RadialBackground) && !IsValid(BackgroundMaterial))
	{
		BackgroundMaterial = RadialBackground->GetDynamicMaterial(); 
		BackgroundMaterial->SetScalarParameterValue(FName("Segments"), EquipmentSlotsSegments.Num()); 
	}

	for (int i = 0; i < EquipmentSlotsSegments.Num(); ++i)
	{
		FName WidgetName = FName(FString::Printf(TEXT("ImageSegment%i"), i)); 
		UImage* WeaponImage = WidgetTree->FindWidget<UImage>(WidgetName); 
		if (!IsValid(WeaponImage))
		{
			continue; 
		}

		FWeaponTableRow* WeaponData = GetTableRowForSegment(i); 
		if (WeaponData == nullptr)
		{
			WeaponImage->SetOpacity(0.0f); 
		}
		else
		{
			WeaponImage->SetOpacity(1.0f);
			WeaponImage->SetBrushFromTexture(WeaponData->WeaponItemDesctiption.Icon); 
		}


	}

}

void UWeaponWheelWidget::SelectSegment()
{
	BackgroundMaterial->SetScalarParameterValue(FName("Index"), CurrentSegmentIndex); 
	FWeaponTableRow* WeaponData = GetTableRowForSegment(CurrentSegmentIndex); 
	if (WeaponData == nullptr)
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Hidden); 
	}
	else
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Visible); 
		WeaponNameText->SetText(WeaponData->WeaponItemDesctiption.Name); 
	}

}

FWeaponTableRow* UWeaponWheelWidget::GetTableRowForSegment(int32 SegmentIndex) const
{
	const EEquipmentSlots& SegmentSlot = EquipmentSlotsSegments[SegmentIndex]; 
	AEquipableItem* EquipableItem = LinkedEquipmentComponent->GetItems()[(int32)SegmentSlot]; 
	if (!IsValid(EquipableItem))
	{
		return nullptr; 
	}
	return XYZ_DataTableUtils::FindWeaponData(EquipableItem->GetDataTableID()); 


}

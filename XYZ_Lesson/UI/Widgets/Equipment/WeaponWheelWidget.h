// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XYZTypes.h"
#include "WeaponWheelWidget.generated.h"

/**
 * 
 */
 

class UCharacterEquipmentComponent; 
struct FWeaponTableRow; 
class UImage; 
class UTextBlock;  

UCLASS()
class XYZ_LESSON_API UWeaponWheelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent); 
	void NextSegment(); 
	void PreviousSegment();
	void ConfirmSelection(); 

protected: 
	virtual void NativeConstruct() override; 
	void SelectSegment(); 

	UPROPERTY(meta = (BindWidget))
		UImage* RadialBackground; 

	UPROPERTY(meta = (BindWidget))
		UTextBlock* WeaponNameText; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon wheel settings")
		TArray<EEquipmentSlots> EquipmentSlotsSegments; 

private: 
	FWeaponTableRow* GetTableRowForSegment(int32 SegmentIndex) const;   

	int32 CurrentSegmentIndex; 
	UMaterialInstanceDynamic* BackgroundMaterial;  
	TWeakObjectPtr<UCharacterEquipmentComponent> LinkedEquipmentComponent; 
	
};

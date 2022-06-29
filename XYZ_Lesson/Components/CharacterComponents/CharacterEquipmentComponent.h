// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "XYZTypes.h"
#include "Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;
typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChanged, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*);

class UEquipmentViewWidget; 
class AThrowableItem;
class ARangeWeaponItem; 
class AMeleeWeaponItem;
class UWeaponWheelWidget; 

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_LESSON_API UCharacterEquipmentComponent : public UActorComponent, public ISaveSubsystemInterface
{
	GENERATED_BODY()


public:
	UCharacterEquipmentComponent();

	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const; 

	AMeleeWeaponItem* GetCurrentMeleeWeapon() const; 

	bool IsEquipping() const; 

	void ReloadCurrentWeapon();

	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	FOnCurrentWeaponAmmoChanged OnCurrentWeaponAmmoChangedEvent;

	FOnEquippedItemChanged OnEquippedItemChanged;

	void EquipItemInSlot(EEquipmentSlots Slot);

	void EquipNextItem();
	void EquipPreviousItem();

	void UnEquipCurrentItem();
	void AttachCurrentItemToEquippedSocket();

	void LaunchCurrentThrowable();

	bool AddEquipmentItemToSlot(const TSubclassOf<AEquipableItem>& EquipableItemClass, int32 SlotIndex); 
	void RemoveItemFromSlot(int32 SlotIndex); 
	void OpenViewEquipment(APlayerController* PlayerController); 
	void CloseViewEquipment(); 
	bool IsViewVisible() const;

	virtual void OnLevelDeserialized_Implementation() override;


	//Weapon Wheel ------------------------------------------------------------------------------------
	void OpenWeaponWheel(APlayerController* PlayerController); 
	bool IsSelectingWeapon() const; 
	void ConfirmWeaponSelection() const; 


	const TArray<AEquipableItem*>& GetItems() const; 


protected:

	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
		TSet<EEquipmentSlots> IgnoreSlotsWhileSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
		EEquipmentSlots AutoEquipItemInSlot = EEquipmentSlots::None; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View")
		TSubclassOf<UEquipmentViewWidget> ViewWidgetClass; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "View")
		TSubclassOf<UWeaponWheelWidget> WeaponWheelClass; 

	void CreateEquipmentWidgets(APlayerController* PlayerController);



private:

	UFUNCTION(Server, Reliable)
		void Server_EquipItemInSlot(EEquipmentSlots Slot);

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadedHandle;

	UPROPERTY(Replicated, SaveGame)
	TArray<int32> AmunitionArray;

	UPROPERTY(ReplicatedUsing=OnRep_ItemsArray, SaveGame)
	TArray<AEquipableItem*> ItemsArray;

	UFUNCTION()
		void OnRep_ItemsArray();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviouItemsArraySlotIndex(uint32 CurrentSlotIndex);

	void CreateLoadout();

	void AutoEquip(); 

	void  EquipAnimationFinished();

	int32 GetAvailableAmunitionForCurrentWeapon();

	

	UFUNCTION()
		void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
		void OnWeaponReloadComplete();

	bool bIsEquipping = false;

	FTimerHandle EquipTimer;

	UEquipmentViewWidget* ViewWidget; 

	EEquipmentSlots PreviousEquippedSlot;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentEquippedSlot, SaveGame)
	EEquipmentSlots CurrentEquippedSlot;

	UFUNCTION()
		void OnRep_CurrentEquippedSlot(EEquipmentSlots CurrentEquippedSlot_Old);

	AEquipableItem* CurrentEquippedItem;

	ARangeWeaponItem* CurrentEquippedWeapon;
	AThrowableItem* CurrentThrowableItem;
	AMeleeWeaponItem* CurrentMeleeWeapon;

	TWeakObjectPtr<class AXYZ_BaseCharacter> CachedBaseCharacter;

	UWeaponWheelWidget* WeaponWheelWidget; 

};

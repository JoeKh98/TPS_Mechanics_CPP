// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "XYZ_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_LESSON_API AXYZ_PlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	virtual void SetPawn(APawn* InPawn) override;

	bool GetIgnoreCameraPitch() const; 

	void SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In);

	virtual void Tick(float DeltaSeconds) override; 

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
		TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
		TSubclassOf<class UUserWidget> MainMenuWidgetClass;

	virtual void SetupInputComponent() override;


private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void Mantle();

	void Jump();
	void ChangeCrouchState();

	void StartSprint();
	void StopSprint();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void ClimbLadderUp(float Value);

	void InteractWithLadder();

	void PlayerStartFire();
	void PlayerStopFire();

	void StartAiming();
	void StopAiming();

	void NextItem();
	void PreviousItem();

	void Reload();
	void EquipPrimaryItem();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	void ToggleMainMenu(); 

	void Interact(); 

	void UseInventory(); 
	void ConfirmWeaponWheelSelection(); 

	void QuickSaveGame(); 
	void QuickLoadGame(); 

	TSoftObjectPtr<class AXYZ_BaseCharacter> CachedBaseCharacter; 

private: 

	void OnInteractableObjectFound(FName ActionName);

	void CreateAndIntitializeWidgets();

	UPlayerHUDWidget* PlayerHUDWidget = nullptr;
	UUserWidget* MainMenuWidget = nullptr; 

	bool IgnoreCameraPitch = false; 
};

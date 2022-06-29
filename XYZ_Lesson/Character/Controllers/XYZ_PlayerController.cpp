// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_PlayerController.h"
#include "../XYZ_BaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widgets/ReticleWidget.h" 
#include "UI/Widgets/AmmoWidget.h" 
#include "UI/Widgets/PlayerHUDWidget.h" 
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "XYZTypes.h"
#include "GameFramework/PlayerInput.h"
#include "Subsystems/SaveSubsystem/SaveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "SignificanceManager.h"

void AXYZ_PlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	CachedBaseCharacter = Cast<AXYZ_BaseCharacter>(InPawn);

	if (CachedBaseCharacter.IsValid() && IsLocalController())
	{
		CreateAndIntitializeWidgets();
		CachedBaseCharacter->OnInteractableObjectFound.BindUObject(this, &AXYZ_PlayerController::OnInteractableObjectFound);
	}
}

bool AXYZ_PlayerController::GetIgnoreCameraPitch() const
{
	return IgnoreCameraPitch;
}

void AXYZ_PlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	IgnoreCameraPitch = bIgnoreCameraPitch_In;
}

void AXYZ_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	USignificanceManager* SignificanceManager = FSignificanceManagerModule::Get(GetWorld()); 
	if (IsValid(SignificanceManager))
	{
		FVector ViewLocation; 
		FRotator ViewRotation; 
		GetPlayerViewPoint(ViewLocation, ViewRotation);

		FTransform ViewTransform(ViewRotation, ViewLocation);
		TArray<FTransform> ViewPoints = { ViewTransform }; 

		SignificanceManager->Update(ViewPoints); 

	}

}

void AXYZ_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
		InputComponent->BindAxis("MoveForward", this, &AXYZ_PlayerController::MoveForward);
		InputComponent->BindAxis("MoveRight", this, &AXYZ_PlayerController::MoveRight);
		InputComponent->BindAxis("Turn", this, &AXYZ_PlayerController::Turn);
		InputComponent->BindAxis("LookUp", this, &AXYZ_PlayerController::LookUp);
		InputComponent->BindAxis("SwimForward", this, &AXYZ_PlayerController::SwimForward);
		InputComponent->BindAxis("SwimRight", this, &AXYZ_PlayerController::SwimRight);
		InputComponent->BindAxis("SwimUp", this, &AXYZ_PlayerController::SwimUp);
		InputComponent->BindAxis("ClimbLadderUp", this, &AXYZ_PlayerController::ClimbLadderUp);

		InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Released, this, &AXYZ_PlayerController::InteractWithLadder);
		InputComponent->BindAction("Mantle", EInputEvent::IE_Released, this, &AXYZ_PlayerController::Mantle);
		InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::Jump);
		InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::ChangeCrouchState);
		InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::StartSprint);
		InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, & AXYZ_PlayerController::StopSprint);
		InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::PlayerStartFire);
		InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AXYZ_PlayerController::PlayerStopFire);
		InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::StartAiming);
		InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AXYZ_PlayerController::StopAiming);
		InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::Reload);
		InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::NextItem);
		InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::PreviousItem);
		InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::EquipPrimaryItem);
		InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::PrimaryMeleeAttack);
		InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::SecondaryMeleeAttack);
		InputComponent->BindAction("UseInventory", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::UseInventory);
		InputComponent->BindAction(ActionInteract, EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::Interact);
		InputComponent->BindAction("ConfirmWeaponWheelSelection", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::ConfirmWeaponWheelSelection);
		InputComponent->BindAction("QuickSaveGame", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::QuickSaveGame);
		InputComponent->BindAction("QuickLoadGame", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::QuickLoadGame);
		FInputActionBinding& ToggleMenuBinding = InputComponent->BindAction("ToggleMainMenu", EInputEvent::IE_Pressed, this, &AXYZ_PlayerController::ToggleMainMenu);
		ToggleMenuBinding.bExecuteWhenPaused = true;


}

void AXYZ_PlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AXYZ_PlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AXYZ_PlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AXYZ_PlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AXYZ_PlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AXYZ_PlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AXYZ_PlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AXYZ_PlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AXYZ_PlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AXYZ_PlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AXYZ_PlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AXYZ_PlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AXYZ_PlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AXYZ_PlayerController::PlayerStartFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AXYZ_PlayerController::PlayerStopFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AXYZ_PlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AXYZ_PlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void AXYZ_PlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AXYZ_PlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AXYZ_PlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void AXYZ_PlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AXYZ_PlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void AXYZ_PlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void AXYZ_PlayerController::ToggleMainMenu()
{
	if (!IsValid(MainMenuWidget) || !IsValid(PlayerHUDWidget))
	{
		return;
	}

	if (MainMenuWidget->IsVisible())
	{
		MainMenuWidget->RemoveFromParent();
		PlayerHUDWidget->AddToViewport();
		SetInputMode(FInputModeGameOnly{});
		SetPause(false); 
		bShowMouseCursor = false; 
	}
	else
	{
		MainMenuWidget->AddToViewport(); 
		PlayerHUDWidget->RemoveFromParent(); 
		SetInputMode(FInputModeGameAndUI{});
		SetPause(true);
		bShowMouseCursor = true; 
	}

}

void AXYZ_PlayerController::Interact()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Interact();
	}
}

void AXYZ_PlayerController::UseInventory()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->UseInventory(this);
	}
}


void AXYZ_PlayerController::ConfirmWeaponWheelSelection()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ConfirmWeaponSelection(); 
	}
}

void AXYZ_PlayerController::QuickSaveGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->SaveGame();
}

void AXYZ_PlayerController::QuickLoadGame()
{
	USaveSubsystem* SaveSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<USaveSubsystem>();
	SaveSubsystem->LoadLastGame();
}

void AXYZ_PlayerController::OnInteractableObjectFound(FName ActionName)
{
	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}

	TArray<FInputActionKeyMapping> ActionKeys = PlayerInput->GetKeysForAction(ActionName);
	const bool HasAnyKeys = ActionKeys.Num() != 0; 
	if (HasAnyKeys)
	{
		FName ActionKey = ActionKeys[0].Key.GetFName(); 
		PlayerHUDWidget->SetHighlightInteractableActionText(ActionKey);

	}
	PlayerHUDWidget->SetHighlightInteractableVisibility(HasAnyKeys);

}

void AXYZ_PlayerController::CreateAndIntitializeWidgets()
{

	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);

		if (IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (!IsValid(MainMenuWidget))
	{
		MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
	}

	if (IsValid(PlayerHUDWidget) && CachedBaseCharacter.IsValid())
	{
		
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();

		if (IsValid(ReticleWidget))
		{

			CachedBaseCharacter->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnEquippedItemChanged.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
		}

		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();

		if (IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
		}

	}

	SetInputMode(FInputModeGameOnly{});
	bShowMouseCursor = false;

}

void AXYZ_PlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

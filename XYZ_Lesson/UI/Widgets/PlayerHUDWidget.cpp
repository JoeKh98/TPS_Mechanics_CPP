// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "Blueprint/WidgetTree.h"
#include "UI/Widgets/HUD/HighlightInteractable.h"


UReticleWidget* UPlayerHUDWidget::GetReticleWidget() const
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget() const
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

void UPlayerHUDWidget::SetHighlightInteractableVisibility(bool bIsVisible)
{


	if (!IsValid(InteractableKey))
	{
		return;
	}

	if (bIsVisible)
	{
		InteractableKey->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractableKey->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUDWidget::SetHighlightInteractableActionText(FName KeyName)
{
	if (IsValid(InteractableKey))
	{
		InteractableKey->SetActionText(KeyName);
	}
}

float UPlayerHUDWidget::GetHealthPercent() const
{
	float Result = 1.0f; 
	APawn* Pawn = GetOwningPlayerPawn();
	AXYZ_BaseCharacter* Character = Cast<AXYZ_BaseCharacter>(Pawn);

	if (IsValid(Character))
	{
		const UCharacterAttributesComponent* CharacterAttributes = Character->GetCharacterAttributesComponent();
		Result = CharacterAttributes->GetHealthPercent();
	}

	return Result;
}

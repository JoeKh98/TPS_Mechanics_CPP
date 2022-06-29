// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"


/**
 * 
 */

class UHighlightInteractable; 

UCLASS()
class XYZ_LESSON_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	class UReticleWidget* GetReticleWidget() const;

	class UAmmoWidget* GetAmmoWidget() const;

	void SetHighlightInteractableVisibility(bool bIsVisible);

	void SetHighlightInteractableActionText(FName KeyName);

protected:

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
		FName ReticleWidgetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names")
		FName AmmoWidgetName;

	UPROPERTY(meta = (BindWidget))
		UHighlightInteractable* InteractableKey; 
	
};

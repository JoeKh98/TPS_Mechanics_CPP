// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Pickables/PickableItem.h"
#include "PickablePowerups.generated.h"

/**
 * 
 */

class AXYZ_BaseCharacter; 

UCLASS(Blueprintable)
class XYZ_LESSON_API APickablePowerups : public APickableItem
{
	GENERATED_BODY()

public: 
	APickablePowerups();

	virtual void Interact(AXYZ_BaseCharacter* Character) override; 
	virtual FName GetActionEventName() const override; 

protected: 
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* PowerupMesh; 

	
};

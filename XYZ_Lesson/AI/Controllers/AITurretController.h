// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XYZ_AIController.h"
#include "AITurretController.generated.h"

/**
 * 
 */

class ATurret; 
UCLASS()
class XYZ_LESSON_API AAITurretController : public AXYZ_AIController
{
	GENERATED_BODY()

public: 



	virtual void SetPawn(APawn* InPawn) override;

	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdateActors) override; 

private:

	TWeakObjectPtr<ATurret> CachedTurret; 
	
};

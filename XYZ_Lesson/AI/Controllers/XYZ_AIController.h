// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "XYZ_AIController.generated.h"

class UAISense;

/**
 * 
 */
UCLASS()
class XYZ_LESSON_API AXYZ_AIController : public AAIController
{
	GENERATED_BODY()

public:

	AXYZ_AIController();

protected: 

	AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseClass) const;
	
};

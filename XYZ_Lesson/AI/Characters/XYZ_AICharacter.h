// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/XYZ_BaseCharacter.h"
#include "XYZ_AICharacter.generated.h"

class UBehaviorTree;
class UAIPatrollingComponent; 
/**
 * 
 */
UCLASS(Blueprintable)
class XYZ_LESSON_API AXYZ_AICharacter : public AXYZ_BaseCharacter
{
	GENERATED_BODY()
	
public: 
	AXYZ_AICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrollingComponent* GetPatrollingComponent() const; 

	UBehaviorTree* GetBehaviorTree() const; 

protected: 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UAIPatrollingComponent* PatrollingComponent;   

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
		UBehaviorTree* BehaviorTree;


};

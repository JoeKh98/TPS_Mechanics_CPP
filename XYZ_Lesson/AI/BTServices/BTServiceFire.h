// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTServiceFire.generated.h"


UCLASS()
class XYZ_LESSON_API UBTServiceFire : public UBTService
{
	GENERATED_BODY()

public:
	UBTServiceFire(); 

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
		FBlackboardKeySelector TargetKey; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
		float MaxFireDistance = 800.0f; 
};

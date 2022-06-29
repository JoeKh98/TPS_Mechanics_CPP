// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controllers/XYZ_AIController.h"
#include "XYZ_AICharacterController.generated.h"

class AXYZ_AICharacter; 
/**
 * 
 */
UCLASS()
class XYZ_LESSON_API AXYZ_AICharacterController : public AXYZ_AIController
{
	GENERATED_BODY()

public:

	virtual void SetPawn(APawn* InPawn) override; 

	virtual void ActorsPerceptionUpdated(const TArray<AActor* >& UpdatedActors) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

protected:
	//virtual void BeginPlay() override; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
		float TargetReachRadius = 100.0f; 

	void SetupPatrolling();

private: 

	void TryMoveToNextTarget();

	bool IsTargetReached(FVector TargetLocation) const;

	bool bIsPatrolling = false;

	TWeakObjectPtr<AXYZ_AICharacter> CachedAICharacter; 
	
};

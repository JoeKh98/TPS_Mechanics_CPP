// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_AICharacter.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"

AXYZ_AICharacter::AXYZ_AICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatrolling"));


}

UAIPatrollingComponent* AXYZ_AICharacter::GetPatrollingComponent() const
{
	return PatrollingComponent;
}

UBehaviorTree* AXYZ_AICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}

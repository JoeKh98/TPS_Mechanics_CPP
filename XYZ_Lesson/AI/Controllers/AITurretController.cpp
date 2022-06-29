// Fill out your copyright notice in the Description page of Project Settings.


#include "AITurretController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h" 
#include "AI/Characters/Turret.h"


void AAITurretController::SetPawn(APawn* InPawn)
{

	Super::SetPawn(InPawn);

	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("AAITurretController::SetPawn AITurretController can posses only Turrets"));
		CachedTurret = StaticCast<ATurret*>(InPawn);

	}
	else
	{
		CachedTurret = nullptr; 
	}
	

}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdateActors)
{ 
	Super::ActorsPerceptionUpdated(UpdateActors);
	if (!CachedTurret.IsValid())
	{
		return;
	}

	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	CachedTurret->CurrentTarget = ClosestActor; 
	CachedTurret->OnCurrentTargetSet(); 


}

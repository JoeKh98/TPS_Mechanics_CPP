// Fill out your copyright notice in the Description page of Project Settings.


#include "BTServiceFire.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"


UBTServiceFire::UBTServiceFire()
{
	NodeName = "Fire";
}

void UBTServiceFire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(Blackboard))
	{
		return;
	}

	AXYZ_BaseCharacter* Character = Cast<AXYZ_BaseCharacter>(AIController->GetPawn());

	if (!IsValid(Character))
	{
		return;
	}

	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRangeWeapon(); 

	if (!IsValid(RangeWeapon))
	{
		return; 
	}

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));

	if (!IsValid(CurrentTarget))
	{
		Character->StopFire();
		return;
	}

	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistSq > FMath::Square(MaxFireDistance))
	{
		Character->StopFire();
		return;
	}

	if (!(RangeWeapon->IsReloading() || RangeWeapon->IsFiring()))
	{
		Character->StartFire();
	}



}

// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "XYZTypes.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/XYZ_BaseCharacter.h"

APlatformTrigger::APlatformTrigger()
{
	bReplicates = true;
	NetUpdateFrequency = 2.0f; 
	MinNetUpdateFrequency = 2.0f; 
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	SetRootComponent(TriggerBox);
}

void APlatformTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//FDoRepLifetimeParams RepParams;
	//RepParams.RepNotifyCondition = REPNOTIFY_Always; 
	DOREPLIFETIME(APlatformTrigger, bIsActivated);

}

void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapEnd);
}

void APlatformTrigger::SetIsActivated(bool bIsActivated_In)
{
	
	if (OnTriggerActivated.IsBound())
	{
		OnTriggerActivated.Broadcast(bIsActivated_In);
	}
}

void APlatformTrigger::OnRep_IsActivated(bool bIsActivated_Old)
{
	SetIsActivated(bIsActivated);
}


void APlatformTrigger::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AXYZ_BaseCharacter* OtherPawn = Cast<AXYZ_BaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (OtherPawn->IsLocallyControlled() || GetLocalRole() == ROLE_Authority )
	{
		OverlappedPawns.AddUnique(OtherPawn);

		if (!bIsActivated && OverlappedPawns.Num() > 0)
		{
			bIsActivated = true;
			SetIsActivated(true);
		}
	}
}

void APlatformTrigger::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AXYZ_BaseCharacter* OtherPawn = Cast<AXYZ_BaseCharacter>(OtherActor);
	if (!IsValid(OtherPawn))
	{
		return;
	}

	if (OtherPawn->IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		OverlappedPawns.RemoveSingleSwap(OtherPawn);

		if (bIsActivated && OverlappedPawns.Num() == 0)
		{
			SetIsActivated(false);
			bIsActivated = false;
		}
	}
}

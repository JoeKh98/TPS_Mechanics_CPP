// Fill out your copyright notice in the Description page of Project Settings.


#include "GCBasePawnAnimInstance.h"
#include "../GameModeBasePawn.h"
#include "../../Components/MovementComponents/GCBasePawnMovementComponent.h"

void UGCBasePawnAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	checkf(TryGetPawnOwner()->IsA<AGameModeBasePawn>(), TEXT("UGCBasePawnAnimInstance::NativeBeginPlay() only GameCodeBasePawn can work with UGCBasePawnAnimInstance"));
	CachedBasePawn = StaticCast<AGameModeBasePawn*>(TryGetPawnOwner());

}

void UGCBasePawnAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBasePawn.IsValid())
	{
		return;
	}

	InputForward = CachedBasePawn->GetInputForward();
	InputRight = CachedBasePawn->GetInputRight();

	bIsInAir = CachedBasePawn->GetMovementComponent()->IsFalling();
	 
}
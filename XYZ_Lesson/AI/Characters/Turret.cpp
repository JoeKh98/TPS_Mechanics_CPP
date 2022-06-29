// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);

	TurretBarellComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarell"));
	TurretBarellComponent->SetupAttachment(TurretBaseComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(TurretBarellComponent);

	SetReplicates(true);

}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, CurrentTarget);

}

void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentTurretState)
	{
	case ETurretState::Searching:
		SearchingMovement(DeltaTime);
		break;
	case ETurretState::Firing:
		FiringMovement(DeltaTime);
		break;
	}
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarell->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarell->GetComponentRotation();
}

void ATurret::OnCurrentTargetSet()
{
	
	
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Firing : ETurretState::Searching;

	SetCurrentTurretState(NewState);
}

void ATurret::SearchingMovement(float DeltaTime)
{

	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate; 
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);

	FRotator TurretBarellRotation = TurretBarellComponent->GetRelativeRotation();
	TurretBarellRotation.Pitch = FMath::FInterpTo(TurretBarellRotation.Pitch, 0.0f, DeltaTime, BarellPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(TurretBarellRotation);

}

void ATurret::FiringMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseFiringInterpSpeed);

	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarellLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarellComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPitchAngle = BarellLookAtDirection.ToOrientationRotator().Pitch;

	FRotator BarellLocalRotation = TurretBarellComponent->GetRelativeRotation();
	BarellLocalRotation.Pitch = FMath::FInterpTo(BarellLocalRotation.Pitch, LookAtPitchAngle, DeltaTime, BarellPitchRotationRate);
	TurretBarellComponent->SetRelativeRotation(BarellLocalRotation);


}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentTurretState;
	CurrentTurretState = NewState;

	if (!bIsStateChanged)
	{
		return;
	}

	switch (CurrentTurretState)
	{
		case ETurretState::Firing:
		{
			GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelayTime);
			break;
		}
		case ETurretState::Searching:
		{
			GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
			break;
		}

	}

}

void ATurret::MakeShot()
{
	FVector ShotLocation = WeaponBarell->GetComponentLocation();
	FVector ShotDirection = WeaponBarell->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpreadAngle = FMath::DegreesToRadians(BulletSpreadAngle); 

	WeaponBarell->Shot(ShotLocation, ShotDirection, SpreadAngle);
}

float ATurret::GetFireInterval() const
{
	return 60.0 / RateOfFire;
}

void ATurret::OnRep_CurrentTarget()
{
	OnCurrentTargetSet();
}




// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"
#include "Components/ExplosionComponents/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{

	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(GetRootComponent());



}

void AExplosiveProjectile::OnProjectileLaunched()
{

	Super::OnProjectileLaunched();
	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapsed, DetonationTime, false);

}

void AExplosiveProjectile::OnDetonationTimerElapsed()
{

	ExplosionComponent->Explode(GetController());

}

AController* AExplosiveProjectile::GetController()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr; 
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBarellComponent.h"
#include "XYZTypes.h"
#include "DrawDebugHelpers.h"
#include "Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Actors/Projectiles/XYZ_Projectile.h"
#include "Net/UnrealNetwork.h"

UWeaponBarellComponent::UWeaponBarellComponent()
{
	SetIsReplicatedByDefault(true); 
}

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	TArray<FShotInfo> ShotsInfo; 

	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		ShotDirection = ShotDirection.GetSafeNormal();
		ShotsInfo.Emplace(ShotStart, ShotDirection); //not just add values but also construct them, array will possess an actor
	}

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerShot(ShotsInfo);
	}
	
	ShotInternal(ShotsInfo);
}

void UWeaponBarellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 
	FDoRepLifetimeParams RepParams; 
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(UWeaponBarellComponent, LastShotsInfo, RepParams); 
	DOREPLIFETIME(UWeaponBarellComponent, ProjectilePool);
	DOREPLIFETIME(UWeaponBarellComponent, CurrentProjectileIndex);
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{

	AActor* HitActor = HitResult.GetActor();


	if (GetOwner()->HasAuthority() && IsValid(HitActor))
	{
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;
		HitActor->TakeDamage(DamageAmount, DamageEvent, GetController(), GetOwner());
	}

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());

	if (IsValid(DecalComponent))
	{
		DecalComponent->SetFadeScreenSize(0.0001f);
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFadeOutTime);
	}

}

bool UWeaponBarellComponent::HitScan(FVector ShotStart, OUT FVector& ShotEnd, FVector ShotDirection)
{
	FHitResult ShotResult;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);

	if (bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;
		ProcessHit(ShotResult, ShotDirection);
		

	}
	return bHasHit;
}

void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{

	AXYZ_Projectile* Projectile = ProjectilePool[CurrentProjectileIndex];
	Projectile->SetActorLocation(LaunchStart);
	Projectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
	Projectile->SetProjectileActive(true);
	Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessProjectileHit);
	Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	++CurrentProjectileIndex;

	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
}

void UWeaponBarellComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() < ROLE_Authority)
	{
		return;
	}

	if (!IsValid(ProjectileClass))
	{
		return;
	}

	ProjectilePool.Reserve(ProjectilePoolSize);

	for (int32 i = 0; i < ProjectilePoolSize; ++i)
	{
		AXYZ_Projectile* Projectile = GetWorld()->SpawnActor<AXYZ_Projectile>(ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetOwningPawn());
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}
}

void UWeaponBarellComponent::ShotInternal(const TArray<FShotInfo>& ShotsInfo)
{
	if (GetOwner()->HasAuthority()) //calling from server?
	{
		LastShotsInfo = ShotsInfo;
	}

	FVector MuzzleLocation = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), MuzzleFlashFX, MuzzleLocation, GetComponentRotation());

	for (const FShotInfo& ShotInfo : ShotsInfo)
	{
		FVector ShotStart = ShotInfo.GetLocation();
		FVector ShotDirection = ShotInfo.GetDirection();
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;
		 


#if ENABLE_DRAW_DEBUG
		UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
		bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRangeWeapon);
#else
		bool bIsDebugEnabled = false;
#endif  

		switch (HitRegistration)
		{

		case EHitRegistrationType::HitScan:
		{
			bool bHasHit = HitScan(ShotStart, ShotEnd, ShotDirection);

			if (bIsDebugEnabled && bHasHit)
			{
				DrawDebugSphere(GetWorld(), ShotEnd, 10.0f, 24, FColor::Red, false, 1.0f);
			}
			break;
		}
		case EHitRegistrationType::Projectile:
		{
			LaunchProjectile(ShotStart, ShotDirection);
			break;
		}
		}

		
		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
		
		if (IsValid(TraceFXComponent))
		{
			TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
		}

		
		if (bIsDebugEnabled)
		{
			DrawDebugLine(GetWorld(), MuzzleLocation, ShotEnd, FColor::Red, false, 1.0f, 0, 3.0f);
		}

	}
}

void UWeaponBarellComponent::ServerShot_Implementation(const TArray<FShotInfo>& ShotsInfo)
{
	ShotInternal(ShotsInfo);
}

void UWeaponBarellComponent::OnRep_LastShotInfo()
{
	ShotInternal(LastShotsInfo);
}

APawn* UWeaponBarellComponent::GetOwningPawn() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}

	return PawnOwner;

}

AController* UWeaponBarellComponent::GetController() const
{
	
	APawn* PawnOwner = GetOwningPawn();

	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;

}

void UWeaponBarellComponent::ProcessProjectileHit(AXYZ_Projectile* Projectile, const FHitResult& HitResult, const FVector& DirectiontResult)
{
	Projectile->SetProjectileActive(false);
	Projectile->SetActorLocation(ProjectilePoolLocation);
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->OnProjectileHit.RemoveAll(this);
	ProcessHit(HitResult, DirectiontResult);
}

FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ
		+ ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;

}

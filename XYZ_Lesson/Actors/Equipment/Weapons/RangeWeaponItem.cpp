// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeaponItem.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "XYZTypes.h"
#include "Character/XYZ_BaseCharacter.h"

ARangeWeaponItem::ARangeWeaponItem()
{

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));

	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	ReticleType = EReticleType::Default;

	EquippedSocketName = SocketCharacterWeapon;
}

void ARangeWeaponItem::StartFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}


	bIsFiring = true;
	MakeShot();

	

}

void ARangeWeaponItem::StopFire() 
{
	bIsFiring = false;
}

bool ARangeWeaponItem::IsFiring() const
{
	return bIsFiring;
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false; 
}

void ARangeWeaponItem::StartReload()
{
	
	AXYZ_BaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return; 
	}

	bIsReloading = true;
	if (IsValid(CharacterReloadMontage))
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);

		if (ReloadType == EReloadType::FullClip)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() { EndReload(true); }, MontageDuration, false);
		}

	}
	else
	{
		EndReload(true);
	}


}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if (!bIsReloading)
	{
		return; 
	}

	AXYZ_BaseCharacter* CharacterOwner = GetCharacterOwner();
	

	if (!bIsSuccess)
	{
		if (IsValid(CharacterOwner))
		{
			CharacterOwner->StopAnimMontage(CharacterReloadMontage);
		}

		StopAnimMontage(WeaponReloadMontage);
	}

	if (ReloadType == EReloadType::ByBullet)
	{
		
		UAnimInstance* CharacterAnimInstance = IsValid(CharacterOwner) ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;

		if (IsValid(CharacterAnimInstance))
		{
			CharacterAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();

		if (IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
		}

	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);

	bIsReloading = false;
	if (bIsSuccess && OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}

}

bool ARangeWeaponItem::IsReloading() const
{
	return bIsFiring;
}

int32 ARangeWeaponItem::GetAmmo() const
{
	return Ammo;
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return MaxAmmo;
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo) 
{
	Ammo = NewAmmo;
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo);
	}
}

EAmunitionType ARangeWeaponItem::GetAmmoType() const
{
	return AmmoType;
}

bool ARangeWeaponItem::CanShoot() const
{
	return Ammo > 0;
}

float ARangeWeaponItem::GetAimFOV() const
{
	return AimFOV;
}

float ARangeWeaponItem::GetAimMovementMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{

	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return bIsAiming ? AimReticleType : ReticleType;
}

void ARangeWeaponItem::OnLevelDeserialized_Implementation()
{
	SetActorRelativeTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector)); 
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo); 
	}
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();

	SetAmmo(MaxAmmo);

}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleInDegrees = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegrees);
}

void ARangeWeaponItem::MakeShot()
{
	AXYZ_BaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	if (!CanShoot())
	{
		StopFire();

		if (Ammo == 0 && bAutoReload)
		{
			CharacterOwner->Reload();
		}

		return;
	}

	EndReload(false);
	CharacterOwner->PlayAnimMontage(CharacterFireMontage);
	PlayAnimMontage(WeaponFireMontage);

	FVector ShotLocation;
	FRotator ShotRotation;

	if (CharacterOwner->IsPlayerControlled())
	{
		APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
		Controller->GetPlayerViewPoint(ShotLocation, ShotRotation);
	}
	else
	{
		ShotLocation = WeaponBarell->GetComponentLocation();
		ShotRotation = CharacterOwner->GetBaseAimRotation();
	}





	FVector ShotDirection = ShotRotation.RotateVector(FVector::ForwardVector);
	
	

	SetAmmo(Ammo - 1);
	WeaponBarell->Shot(ShotLocation, ShotDirection, GetCurrentBulletSpreadAngle());

	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
	
}

void ARangeWeaponItem::OnShotTimerElapsed()
{

	if (!bIsFiring)
	{
		return;
	}

	switch (WeaponFireMode)
	{
		case EWeaponFireMode::Single:
		{
			StopFire();
			break;
		}
		case EWeaponFireMode::FullAuto:
		{
			MakeShot();
			
		}
	}

}



float ARangeWeaponItem::GetShotTimerInterval() const
{
	return 60.0f / RateOfFire; 
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.0f; 

	if (IsValid(WeaponAnimInstance))
	{
		Result =  WeaponAnimInstance->Montage_Play(AnimMontage);
	}

	return Result; 

}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{

	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (IsValid(WeaponAnimInstance))
	{

		WeaponAnimInstance->Montage_Stop(BlendOutTime, AnimMontage);

	}

}

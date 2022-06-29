// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableItem.h"
#include "Character/XYZ_BaseCharacter.h"
#include "Actors/Projectiles/XYZ_Projectile.h"

void AThrowableItem::Throw()
{

	
	AXYZ_BaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	

	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);

	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);

	FVector ViewDirection = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector ViewUpVector = PlayerViewRotation.RotateVector(FVector::UpVector);
	FVector LaunchDirection = ViewDirection + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * ViewUpVector;

	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable); 
	FVector SocketInViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);

	FVector SpawnLocation = PlayerViewPoint + ViewDirection * SocketInViewSpace.X;
	AXYZ_Projectile* Projectile = GetWorld()->SpawnActor<AXYZ_Projectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);

	if (IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	}

}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XYZ_Projectile.generated.h"

class AXYZ_Projectile; 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHit, AXYZ_Projectile*, Projectile, const FHitResult&, Hit, const FVector&, Direction);

UCLASS()
class XYZ_LESSON_API AXYZ_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AXYZ_Projectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

	UPROPERTY(BlueprintAssignable)
		FOnProjectileHit OnProjectileHit;

	UFUNCTION(BlueprintNativeEvent)
		void SetProjectileActive(bool bIsProjectileActive); 

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class USphereComponent* CollisionComponent; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		class UProjectileMovementComponent* ProjectileMovementComponent; 

	virtual void BeginPlay() override;


	virtual void OnProjectileLaunched();

private:

	UFUNCTION()
		void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};

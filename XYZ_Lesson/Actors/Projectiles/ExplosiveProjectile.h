// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/XYZ_Projectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */

class UExplosionComponent;
UCLASS()
class XYZ_LESSON_API AExplosiveProjectile : public AXYZ_Projectile
{
	GENERATED_BODY()
	
public:

	AExplosiveProjectile();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Explosion")
		float DetonationTime = 2.0f;

	virtual void OnProjectileLaunched() override;

private:

	void OnDetonationTimerElapsed();

	AController* GetController();

	FTimerHandle DetonationTimer;

};

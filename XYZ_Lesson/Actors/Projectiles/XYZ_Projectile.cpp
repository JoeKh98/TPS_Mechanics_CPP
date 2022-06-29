// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AXYZ_Projectile::AXYZ_Projectile()
{
 	
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    //HW preset collision custom
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
    SetRootComponent(CollisionComponent);


    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
    ProjectileMovementComponent->InitialSpeed = 2000.0f;
    ProjectileMovementComponent->bAutoActivate = false; 

    SetReplicates(true);
    SetReplicateMovement(true); //replicates movement
}

void AXYZ_Projectile::LaunchProjectile(FVector Direction)
{

    ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
    CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
    OnProjectileLaunched();
    

}

void AXYZ_Projectile::SetProjectileActive_Implementation(bool bIsProjectileActive)
{
    ProjectileMovementComponent->SetActive(bIsProjectileActive);
}

void AXYZ_Projectile::BeginPlay()
{

    Super::BeginPlay();

    CollisionComponent->OnComponentHit.AddDynamic(this, &AXYZ_Projectile::OnCollisionHit);

}

void AXYZ_Projectile::OnProjectileLaunched()
{
}

void AXYZ_Projectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

    if (OnProjectileHit.IsBound())
    {
        OnProjectileHit.Broadcast(this, Hit, ProjectileMovementComponent->Velocity.GetSafeNormal()); 
    }

}

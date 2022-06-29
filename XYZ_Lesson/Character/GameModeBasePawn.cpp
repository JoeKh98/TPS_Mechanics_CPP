// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeBasePawn.h"
#include "Engine/CollisionProfile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../Components/MovementComponents/GCBasePawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"


// Sets default values
AGameModeBasePawn::AGameModeBasePawn()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	CollisionComponent->SetSphereRadius(CollisionSphereRadius);
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	RootComponent = CollisionComponent;

	//MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UFloatingPawnMovement>(TEXT("Movement Component"));
	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UGCBasePawnMovementComponent>(TEXT("Movement Component"));
	MovementComponent->SetUpdatedComponent(CollisionComponent);

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->bUsePawnControlRotation = 1;
	SpringArmComponent->SetupAttachment(RootComponent);

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(RootComponent);
#endif
}



// Called to bind functionality to input
void AGameModeBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("MoveForward", this, & AGameModeBasePawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGameModeBasePawn::MoveRight);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AGameModeBasePawn::Jump);

}

void AGameModeBasePawn::MoveForward(float val) 
{
	InputForward = val;
	if (val != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorForwardVector(), val);
	}
}

void AGameModeBasePawn::MoveRight(float val)
{
	InputRight = val ;
	if (val != 0.0f)
	{
		AddMovementInput(CurrentViewActor->GetActorRightVector(), val);
	}
}

void AGameModeBasePawn::Jump()
{
	checkf(MovementComponent->IsA<UGCBasePawnMovementComponent>(), TEXT("Jump can work only with UGCBasePawnMovementComponent"));
	UGCBasePawnMovementComponent* BaseMovement = StaticCast<UGCBasePawnMovementComponent*>(MovementComponent);
	BaseMovement->JumpStart();
}

void AGameModeBasePawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	CurrentViewActor = CameraManager->GetViewTarget();
	CameraManager->OnBlendComplete().AddUFunction(this, FName("OnBlendComplete"));

}

void AGameModeBasePawn::OnBlendComplete()
{
	CurrentViewActor = GetController()->GetViewTarget();
}


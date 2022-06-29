// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Subsystems/Streaming/StreamingSubsystemUtils.h"


APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	Team = ETeams::Player;

	//SpringArmComponent->bEnableCameraLag = 1;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SprintSpringArm.TickTimeline(DeltaTime);
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);

}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);

}

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	return bIsCrouched || Super::CanJumpInternal_Implementation();
}

void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void APlayerCharacter::OnSprintStart_Implementation()
{
	SprintSpringArm.Play();
	UE_LOG(LogTemp, Log, TEXT("APlayerCharacter::OnSprintStart_Implementation()"));
	
}

void APlayerCharacter::OnSprintEnd_Implementation()
{
	SprintSpringArm.Reverse();
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::OnSprintEnd_Implementation()"));
	
}

void APlayerCharacter::SwimForward(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);

	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);

	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);

	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UStreamingSubsystemUtils::CheckCharacterOverlapStreamingSubsystemVolume(this);

	
	UpdateFunctionFloat.BindDynamic(this, &APlayerCharacter::UpdateSprintTimeline);

	if (IsValid(SpringArmCurve))
	{
		FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
		PlatformMovementTimelineUpdate.BindUObject(this, &APlayerCharacter::UpdateSprintTimeline);
		SprintSpringArm.AddInterpFloat(SpringArmCurve, UpdateFunctionFloat);

		StartLocation = EndLocation = SpringArmComponent->TargetArmLength;

		EndLocation += Offset;
	}

	

	
	
}

void APlayerCharacter::UpdateSprintTimeline(float Alpha)
{
	//SpringArmComponent->TargetArmLength = EndLocation;
	
	float TargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	SpringArmComponent->TargetArmLength = TargetLocation;

	
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal(); 
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
		CameraManager->SetFOV(CurrentRangeWeapon->GetAimFOV());
	}

}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
		CameraManager->UnlockFOV();
	}

}



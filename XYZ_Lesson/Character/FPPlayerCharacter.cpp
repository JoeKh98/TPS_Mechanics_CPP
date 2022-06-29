// Fill out your copyright notice in the Description page of Project Settings.


#include "FPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../XYZTypes.h"
#include "Controllers/XYZ_PlayerController.h"
#include "../Components/MovementComponents/BaseCharacterMovementComponent.h"

AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -86.0f));
	FirstPersonMeshComponent->CastShadow = false; 
	FirstPersonMeshComponent->bCastDynamicShadow = false; 
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true; 

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;
	
	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false; 

	bUseControllerRotationYaw = true;

}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	GCPlayerController = Cast<AXYZ_PlayerController>(NewController);

}

void AFPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsFPMontagePlaying() && GCPlayerController.IsValid())
	{
		FRotator TargetControlRotation = GCPlayerController->GetControlRotation();
		TargetControlRotation.Pitch = 0.0f;
		float BlendSpeed = 300.0f;
		TargetControlRotation = FMath::RInterpTo(GCPlayerController->GetControlRotation(), TargetControlRotation, DeltaTime, BlendSpeed);
		GCPlayerController->SetControlRotation(TargetControlRotation);
	}
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// returns blueprint class (GEtCLASS())
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());

	//Returns non const reference
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();

	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;


}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	
	// returns blueprint class (GEtCLASS())
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());

	//Returns non const reference
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();

	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;

}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();

	if (IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Pitch += SocketRotation.Pitch;
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
	}

	return Result;
}


void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)  
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		OnLadderStarted();
	}
	else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		OnLadderStopped();
	}
}

void AFPPlayerCharacter::OnLadderStopped()
{
	if (GCPlayerController.IsValid())
	{
		GCPlayerController->SetIgnoreCameraPitch(false);
		bUseControllerRotationYaw = true;
		APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
		APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();

		CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
		CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
		CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
		CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
	}
}

void AFPPlayerCharacter::OnLadderStarted()
{
	if (GCPlayerController.IsValid())
	{
		GCPlayerController->SetIgnoreCameraPitch(true);
		bUseControllerRotationYaw = false;
		APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
		CameraManager->ViewPitchMin = LadderCameraMinPitch;
		CameraManager->ViewPitchMax = LadderCameraMaxPitch;
		CameraManager->ViewYawMin = LadderCameraMinYaw;
		CameraManager->ViewYawMax = LadderCameraMaxYaw;
	}
}



void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);

	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && MantlingSettings.FPMantlingMontage)
	{
		
		if (GCPlayerController.IsValid())
		{
			GCPlayerController->SetIgnoreLookInput(true);
			GCPlayerController->SetIgnoreMoveInput(true);
		}
		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);
		GetWorld()->GetTimerManager().SetTimer(FPMontageTimer, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
	 

}

void AFPPlayerCharacter::OnFPMontageTimerElapsed()
{
	
	if (GCPlayerController.IsValid())
	{
		GCPlayerController->SetIgnoreLookInput(false);
		GCPlayerController->SetIgnoreMoveInput(false);
	}
	

}

bool AFPPlayerCharacter::IsFPMontagePlaying() const 
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();


}

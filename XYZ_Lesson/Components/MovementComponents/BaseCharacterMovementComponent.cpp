// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../../Actors/Interactive/Environment/Ladder.h"
#include "../../Character/XYZ_BaseCharacter.h"
#include "Curves/CurveVector.h"

FNetworkPredictionData_Client* UBaseCharacterMovementComponent::GetPredictionData_Client() const
{
    if (ClientPredictionData == nullptr)
    {
        UBaseCharacterMovementComponent* MutableThis = const_cast<UBaseCharacterMovementComponent*>(this);
        MutableThis->ClientPredictionData = new NetworkPredictionData_Client_Character_XYZ(*this);
    }
    return ClientPredictionData;
}

void UBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
    /*
    FLAG_Reserved_1		= 0x04,	// Reserved for future use
    FLAG_Reserved_2		= 0x08,	// Reserved for future use
    // Remaining bit masks are available for custom flags.
    FLAG_Custom_0		= 0x10, - Sprinting Flag !!!!
    FLAG_Custom_1		= 0x20, - Mantling Flag !!!!
    FLAG_Custom_2		= 0x40,
    FLAG_Custom_3		= 0x80,
    */
    Super::UpdateFromCompressedFlags(Flags);

    bool bWasMantling = GetBaseCharacterOwner()->bIsMantling;
    bIsSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
    bool bIsMantling = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;

    if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_Authority)
    {
        if (!bWasMantling && bIsMantling)
        {
            GetBaseCharacterOwner()->Mantle(true);
        }
    }

}

void UBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
    if (bForceRotation)
    {
        FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
        CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

        FRotator DeltaRot = GetDeltaRotation(DeltaTime);
        DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

        // Accumulate a desired new rotation.
        const float AngleTolerance = 1e-3f;

        if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
        {

            FRotator DesiredRotation = ForceTargetRotation;
            // PITCH
            if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
            {
                DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
            }

            // YAW
            if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
            {
                DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
            }

            // ROLL
            if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
            {
                DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
            }

            // Set the new rotation.
            DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
            MoveUpdatedComponent(FVector::ZeroVector, DesiredRotation, /*bSweep*/ false);
        }
        else
        {

            ForceTargetRotation = FRotator::ZeroRotator;
            bForceRotation = false;
        }
        return;

    }

    if (IsOnLadder())
    {
        return;
    }
    Super::PhysicsRotation(DeltaTime);
}

float UBaseCharacterMovementComponent::GetMaxSpeed() const
{
    float Result = Super::GetMaxSpeed();
    if (bIsSprinting)
    {
        Result = SprintSpeed;
    }
    else if (bIsOutOfStamina)
    {
        Result = OutOfStaminaSpeed;
    }
    else if (IsOnLadder())
    {
        Result = ClimbingOnLadderMaxSpeed;
    }
    else if (GetBaseCharacterOwner()->IsAiming())
    {
        Result = GetBaseCharacterOwner()->GetAimingMovementSpeed();
    }
    return Result;
}

void UBaseCharacterMovementComponent::StartSprint()
{
    bIsSprinting = true;
    bForceMaxAccel = 1;
}

void UBaseCharacterMovementComponent::StopSprint()
{
    bIsSprinting = false;
    bForceMaxAccel = 0;
}

void UBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParams& MantlingParams)
{
    CurrentMantlingParams = MantlingParams;
    SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);

}

void UBaseCharacterMovementComponent::EndMantle()
{
    GetBaseCharacterOwner()->bIsMantling = false; 
    SetMovementMode(MOVE_Walking);
}

bool UBaseCharacterMovementComponent::IsMantling() const
{

    return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling ;
}

void UBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

    if (MovementMode == MOVE_Swimming)
    {
        CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight );
    }
    else if (PreviousMovementMode == MOVE_Swimming)
    {
        ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
        CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

    }

    if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
    {
        CurrentLadder = nullptr;
    }

    if (MovementMode == MOVE_Custom)
    {
        switch (CustomMovementMode)
        {
            case (uint8)ECustomMovementMode::CMOVE_Mantling:
            {
                
                GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UBaseCharacterMovementComponent::EndMantle, CurrentMantlingParams.Duration, false);
                break;
            }
             
        default:
            break;
        }
    }
}

void UBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
    CurrentLadder = Ladder; 

    FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
    TargetOrientationRotation.Yaw += 180.0f;


    /* Changing Character location according to Ladder */
   
   
    FVector LadderUpVector = CurrentLadder->GetActorUpVector();
    FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
    float Projection = GetActorToCurrentLadderProjection(GetActorLocation());

    FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * CurrentLadder->GetActorForwardVector();
    
    if (CurrentLadder->GetIsOnTop())
    {

        NewCharacterLocation = CurrentLadder->GetAttachTopAnimMontageStartingLocation();

    }

    GetOwner()->SetActorLocation(NewCharacterLocation);
    GetOwner()->SetActorRotation(TargetOrientationRotation);

    SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
}

float UBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
    checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection() cant be invoked when current ladder is null"));

    FVector LadderUpVector = CurrentLadder->GetActorUpVector();
    FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();

    return  FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

float UBaseCharacterMovementComponent::GetLadderSpeedRatio() const
{
    checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetLadderSpeedRatio()"));

    FVector LadderUpVector = CurrentLadder->GetActorUpVector();

    return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderMaxSpeed;

}

void UBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod  /* = EDetachFromLadderMethod::Fall */)
{
    switch (DetachFromLadderMethod)
    {
    case EDetachFromLadderMethod::JumpOff:
    {
        FVector JumpDirection = CurrentLadder->GetActorForwardVector();

        SetMovementMode(MOVE_Falling);

        FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;

        ForceTargetRotation = JumpDirection.ToOrientationRotator();
        bForceRotation = true;

        Launch(JumpVelocity);
        break; 
    }


    case EDetachFromLadderMethod::ReachingTheTop:
    {
        GetBaseCharacterOwner()->Mantle(true);
        break;  
    }

    case EDetachFromLadderMethod::ReachingTheBottom:
    {
        SetMovementMode(MOVE_Walking);
        break;
    }

    case EDetachFromLadderMethod::Fall:
    default:
    {
        SetMovementMode(MOVE_Falling);
        break;
    }

   
    }
   
    
}

bool UBaseCharacterMovementComponent::IsOnLadder() const
{
    return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

const ALadder* UBaseCharacterMovementComponent::GetCurrentLadder()
{
    return CurrentLadder;
}

void UBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
    if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
    {
        return;
    }

    switch (CustomMovementMode)
    {
    case (uint8)ECustomMovementMode::CMOVE_Mantling:
    {
        PhysMantling(DeltaTime, Iterations);
        break;
    }
    case (uint8)ECustomMovementMode::CMOVE_Ladder:
    {
        PhysLadder(DeltaTime, Iterations);
        break;
    }
    default:
        break;
    }

    Super::PhysCustom(DeltaTime, Iterations);

}

void UBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
    float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParams.StartTime;

    FVector MantlingCurveValue = CurrentMantlingParams.MantlingCurve->GetVectorValue(ElapsedTime);

    float PositionAlpha = MantlingCurveValue.X;
    float XYCorrectionAlpha = MantlingCurveValue.Y;
    float ZCorrectionAlpha = MantlingCurveValue.Z;

    FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParams.InitialLocation, CurrentMantlingParams.InitialAnimationLocation, XYCorrectionAlpha);

    CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParams.InitialLocation.Z, CurrentMantlingParams.InitialAnimationLocation.Z, ZCorrectionAlpha);

    FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParams.TargetLocation, PositionAlpha);
    FRotator NewRotation = FMath::Lerp(CurrentMantlingParams.InitialRotation, CurrentMantlingParams.TargetRotation, PositionAlpha);

    FVector Delta = NewLocation - GetActorLocation();
    Velocity = Delta / DeltaTime;

    FHitResult Hit;

    SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
    CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBrakingDeceleration);
    FVector Delta = Velocity * DeltaTime; 

    if (HasAnimRootMotion())
    {
        FHitResult Hit;
        SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
        return;

    }


    FVector NewPos = GetActorLocation() + Delta;
    float NewPosProjection = GetActorToCurrentLadderProjection(NewPos); 

    if (NewPosProjection < MinLadderBottomOffset)
    {
        DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
        return;
    }
    else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
    {
        DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
        return;
    }

    FHitResult Hit;
    SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

AXYZ_BaseCharacter* UBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
    return StaticCast<AXYZ_BaseCharacter*>(CharacterOwner);
}

void FSavedMove_XYZ::Clear()
{
    Super::Clear();
    bSavedIsSprinting = 0; 
    bSavedIsMantling = 0;

}

uint8 FSavedMove_XYZ::GetCompressedFlags() const
{
    uint8 Result = Super::GetCompressedFlags(); 

    /*  
        FLAG_Reserved_1		= 0x04,	// Reserved for future use
		FLAG_Reserved_2		= 0x08,	// Reserved for future use
		// Remaining bit masks are available for custom flags.
		FLAG_Custom_0		= 0x10, - Sprinting Flag !!!!
		FLAG_Custom_1		= 0x20, - Mantling Flag !!!!
		FLAG_Custom_2		= 0x40,
		FLAG_Custom_3		= 0x80, 
        
    */

    if (bSavedIsSprinting)
    {
        Result |= FLAG_Custom_0; 
    }
    if (bSavedIsMantling)
    {
        Result &= ~FLAG_JumpPressed;
        Result |= FLAG_Custom_1;
    }

    return Result; 
}

bool FSavedMove_XYZ::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
    const FSavedMove_XYZ* NewMove = StaticCast<const FSavedMove_XYZ*>(NewMovePtr.Get());

    if (bSavedIsSprinting != NewMove->bSavedIsSprinting
        || bSavedIsMantling != NewMove->bSavedIsSprinting)
    {
        return false; 
    }

    return Super::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);

}

void FSavedMove_XYZ::SetMoveFor(ACharacter* InCharacter, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
    Super::SetMoveFor(InCharacter, InDeltaTime, NewAccel, ClientData);

    check(InCharacter->IsA<AXYZ_BaseCharacter>())
    AXYZ_BaseCharacter* InBaseCharacter = StaticCast<AXYZ_BaseCharacter*>(InCharacter);
    UBaseCharacterMovementComponent* MovementComponent = InBaseCharacter->GetBaseCharacterMovementComponent();

    bSavedIsSprinting = MovementComponent->bIsSprinting; 
    bSavedIsMantling = InBaseCharacter->bIsMantling;
}

void FSavedMove_XYZ::PrepMoveFor(ACharacter* Character)
{
    Super::PrepMoveFor(Character);

    UBaseCharacterMovementComponent* MovementComponent = StaticCast<UBaseCharacterMovementComponent*>(Character->GetMovementComponent());

    MovementComponent->bIsSprinting = bSavedIsSprinting;
}

NetworkPredictionData_Client_Character_XYZ::NetworkPredictionData_Client_Character_XYZ(const UCharacterMovementComponent& ClientMovement) 
    :Super(ClientMovement)
{
}

FSavedMovePtr NetworkPredictionData_Client_Character_XYZ::AllocateNewMove()
{
    return FSavedMovePtr(new FSavedMove_XYZ());
}

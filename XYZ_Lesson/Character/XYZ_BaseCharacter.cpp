 // Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_BaseCharacter.h"
#include "Components/MovementComponents/BaseCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Actors/Interactive/Environment/Ladder.h" 
#include "Components/LedgeDetectorComponent.h"
#include "Curves/CurveVector.h"
#include "XYZTypes.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Equipment/Melee/MeleeWeaponItem.h"
#include "Actors/Environment/PlatformTrigger.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "Components/WidgetComponent.h" 
#include "UI/Widgets/World/XYZ_AttributeProgressBar.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"





AXYZ_BaseCharacter::AXYZ_BaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	BaseCharacterMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));

	GetMesh()->CastShadow = true; 
	GetMesh()->bCastDynamicShadow = true;

	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>(TEXT("CharacterAttributes"));

	CharacterEquipmentComponent = CreateAbstractDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));
	CharacterInventoryComponent = CreateAbstractDefaultSubobject<UCharacterInventoryComponent>(TEXT("CharacterInventory"));

	HealthBarProgressComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarProgressComponent"));
	HealthBarProgressComponent->SetupAttachment(GetCapsuleComponent());
}

void AXYZ_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &AXYZ_BaseCharacter::OnDeath);
	InitializeHealthProgress(); 

	if (bIsSignificanceEnabled)
	{
		USignificanceManager* SignificanceManager = FSignificanceManagerModule::Get(GetWorld());
		if (IsValid(SignificanceManager))
		{
			SignificanceManager->RegisterObject(
				this,
				SignificanceTagCharacter,
				[this](USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& ViewPoint) -> float
				{
					return SignificanceFunction(ObjectInfo, ViewPoint);
				},
				USignificanceManager::EPostSignificanceType::Sequential,
					[this](USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
				{
					PostSignificanceFunction(ObjectInfo, OldSignificance, Significance, bFinal);
				}
				);
		}
	}
}

void AXYZ_BaseCharacter::EndPlay(const EEndPlayReason::Type Reason)
{
	if (OnInteractableObjectFound.IsBound())
	{
		OnInteractableObjectFound.Unbind(); 
	}
	Super::EndPlay(Reason); 

}

void AXYZ_BaseCharacter::OnLevelDeserialized_Implementation()
{
}

void AXYZ_BaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AXYZ_BaseCharacter, bIsMantling);
}

void AXYZ_BaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}

}

void AXYZ_BaseCharacter::ChangeCrouchState()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AXYZ_BaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void AXYZ_BaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AXYZ_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStamina(DeltaTime);

	TryChangeSprintState(DeltaTime);

	UpdateIKSettings(DeltaTime);

	TraceLineOfSight();

}

void AXYZ_BaseCharacter::StartFire()
{
	if (CharacterEquipmentComponent->IsSelectingWeapon())
	{
		return;
	}

	if (CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{

		CurrentRangeWeapon->StartFire();

	}
	

}

void AXYZ_BaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();

	if (IsValid(CurrentRangeWeapon))
	{

		CurrentRangeWeapon->StopFire();

	}
}

void AXYZ_BaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (!IsValid(CurrentRangeWeapon))
	{
		return;
	}

	bIsAiming = true;
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementMaxSpeed();
	CurrentRangeWeapon->StartAim();
	OnStartAiming();

}

void AXYZ_BaseCharacter::StopAiming()
{


	if (!bIsAiming)
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopAim();
	}


	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;
	OnStopAiming();

}

FRotator AXYZ_BaseCharacter::GetAimOffset()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	FRotator Result = AimDirectionLocal.ToOrientationRotator();

	return Result; 
}

void AXYZ_BaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void AXYZ_BaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

bool AXYZ_BaseCharacter::IsAiming() const
{
	return bIsAiming;
}

void AXYZ_BaseCharacter::Reload() 
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadCurrentWeapon();
	}
}

void AXYZ_BaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void AXYZ_BaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();	
}

void AXYZ_BaseCharacter::EquipPrimaryItem()
{

	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);

}

float AXYZ_BaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;

}

void AXYZ_BaseCharacter::Mantle(bool bForce /*= false*/)
{

	if (!(CanMantle() || bForce))
	{
		return;
	}

	FLedgeDescription LedgeDesctiption; 

	if (LedgeDetectorComponent->DetectLedge(LedgeDesctiption))
	{
		bIsMantling = true;

		FMantlingMovementParams MantlingParams;
		MantlingParams.InitialLocation = GetActorLocation();
		MantlingParams.InitialRotation = GetActorRotation();
		MantlingParams.TargetLocation = LedgeDesctiption.Location;
		MantlingParams.TargetRotation = LedgeDesctiption.Rotation;

		float MantlingHeight = (MantlingParams.TargetLocation - MantlingParams.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange; 
		float MaxRange; 

		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);

		MantlingParams.Duration = MaxRange - MinRange;

		

		//float StartTime = MantlingSettings.MinHeightStartTime + (MantlingHeight - MantlingSettings.MinHeight) / (MantlingSettings.MaxHeight - MantlingSettings.MinHeight) * (MantlingSettings.MaxHeightStartTime - MantlingSettings.MinHeightStartTime);

		MantlingParams.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParams.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingParams.InitialAnimationLocation = MantlingParams.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDesctiption.LedgeNormal;

		if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
		{
			GetBaseCharacterMovementComponent()->StartMantle(MantlingParams);
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParams.StartTime);
		 
		OnMantle(MantlingSettings, MantlingParams.StartTime);
	}
}

void AXYZ_BaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy && !bWasMantling && bIsMantling)
	{
		Mantle(true);
	}
}

bool AXYZ_BaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void AXYZ_BaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{

			if (AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}

			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
		
	}

}

void AXYZ_BaseCharacter::ClimbLadderUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

const UCharacterEquipmentComponent* AXYZ_BaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

UCharacterEquipmentComponent* AXYZ_BaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponent;
}

const UCharacterAttributesComponent* AXYZ_BaseCharacter::GetCharacterAttributesComponent() const
{
	return CharacterAttributesComponent;
}

void AXYZ_BaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.AddUnique(InteractiveActor);
}

void AXYZ_BaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

const ALadder* AXYZ_BaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;

	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}

	return Result;
}

void AXYZ_BaseCharacter::Falling()
{
	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void AXYZ_BaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();

}

void AXYZ_BaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;

	if (IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}

}

void AXYZ_BaseCharacter::PrimaryMeleeAttack()
{

	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::PrimaryAttack);
	}

}

void AXYZ_BaseCharacter::SecondaryMeleeAttack()
{

	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeapon();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::SecondaryAttack);
	}

}

void AXYZ_BaseCharacter::Interact()
{
	if (LineOfSightObject.GetInterface()) //some variant of isValid()
	{
		LineOfSightObject->Interact(this);

	}
}



bool AXYZ_BaseCharacter::PickupItem(TWeakObjectPtr<UInventoryItem> ItemToPickup)
{
	bool Result = false;

	if (CharacterInventoryComponent->HasFreeSlot())
	{
		CharacterInventoryComponent->AddItem(ItemToPickup, 1); 
		Result = true; 
	}

	return Result; 
}

void AXYZ_BaseCharacter::UseInventory(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return; 
	}

	

	if (!CharacterInventoryComponent->IsViewVisible())
	{
		CharacterInventoryComponent->OpenViewInventory(PlayerController);
		CharacterEquipmentComponent->OpenViewEquipment(PlayerController); 
		PlayerController->SetInputMode(FInputModeGameAndUI{});
		PlayerController->bShowMouseCursor = true; 
	}
	else
	{
		CharacterInventoryComponent->CloseViewInventory();
		CharacterEquipmentComponent->CloseViewEquipment(); 
		PlayerController->SetInputMode(FInputModeGameOnly{}); 
		PlayerController->bShowMouseCursor = false; 
	}

}

void AXYZ_BaseCharacter::RestoreFullStamina()
{
	CurrentStamina = MaxStamina;
}

void AXYZ_BaseCharacter::AddHealth(float Health)
{
	CharacterAttributesComponent->AddHealth(Health);
}

void AXYZ_BaseCharacter::ConfirmWeaponSelection()
{
	if (CharacterEquipmentComponent->IsSelectingWeapon())
	{
		CharacterEquipmentComponent->ConfirmWeaponSelection(); 
	}
}

FGenericTeamId AXYZ_BaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Team);
}

//void AXYZ_BaseCharacter::Client_ActivatePlatformTrigger_Implementation(APlatformTrigger* PlatformTrigger, bool bIsActivated)
//{
//	PlatformTrigger->SetIsActivated(bIsActivated);
//}
//
//void AXYZ_BaseCharacter::Server_ActivatePlatformTrigger_Implementation(class APlatformTrigger* PlatformTrigger, bool bIsActivated)
//{
//	PlatformTrigger->Multicast_SetIsActivated(bIsActivated);
//}

void AXYZ_BaseCharacter::UpdateStamina(float DeltaTime)
{
	if (FMath::IsNearlyZero(CurrentStamina))
	{
		GetBaseCharacterMovementComponent()->SetIsOutOfStamina(true);
	}

	if (CanRestoreStamina())
	{
		CurrentStamina += StaminaRestoreVelocity * DeltaTime;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
	}

	if (FMath::IsNearlyEqual(CurrentStamina, MaxStamina))
	{
		GetBaseCharacterMovementComponent()->SetIsOutOfStamina(false);
	}

	DebugDrawStamina();
}

void AXYZ_BaseCharacter::TraceLineOfSight()
{
	if (!IsPlayerControlled())
	{
		return;
	}


	FVector ViewLocation;
	FRotator ViewRotation;

	APlayerController* PlayerController = GetController<APlayerController>();
	PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

	//Direction Vector 
	FVector ViewDirection = ViewRotation.Vector();

	FVector TraceEnd = ViewLocation + ViewDirection * LineOfSightDistance;

	FHitResult HitResult;

	GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility); //only objs that we can see 

	if (LineOfSightObject.GetObject() != HitResult.Actor)
	{
		LineOfSightObject = HitResult.Actor.Get();

		FName ActionName;
		if (LineOfSightObject.GetInterface())
		{
			ActionName = LineOfSightObject->GetActionEventName();
		}
		else
		{
			ActionName = NAME_None;
		}

		OnInteractableObjectFound.ExecuteIfBound(ActionName);

	}




}

void AXYZ_BaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void AXYZ_BaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

float AXYZ_BaseCharacter::SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& ViewPoint)
{
	if (ObjectInfo->GetTag() == SignificanceTagCharacter)
	{
		AXYZ_BaseCharacter* Character = StaticCast<AXYZ_BaseCharacter*>(ObjectInfo->GetObject()); 
		if (!IsValid(Character))
		{
			return SignificanceValueVeryHigh;
		}
		if (Character->IsPlayerControlled() && Character->IsLocallyControlled())
		{
			return SignificanceValueVeryHigh; 
		}

		float DistToSquared = FVector::DistSquared(Character->GetActorLocation(), ViewPoint.GetLocation()); 
		if (DistToSquared <= FMath::Square(VeryHighSignificanceDistance))
		{
			return SignificanceValueVeryHigh; 
		}		
		else if (DistToSquared <= FMath::Square(HighSignificanceDistance))
		{
			return SignificanceValueHigh; 
		}
		else if (DistToSquared <= FMath::Square(MediumSignificanceDistance))
		{
			return SignificanceValueMedium; 
		}
		else if (DistToSquared <= FMath::Square(LowSignificanceDistance))
		{
			return SignificanceValueLow; 
		}
		else
		{
			return SignificanceValueVeryLow; 
		}

	}

	return VeryHighSignificanceDistance;
}

void AXYZ_BaseCharacter::PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal)
{
	if (OldSignificance == Significance)
	{
		return; 
	}
	if (ObjectInfo->GetTag() != SignificanceTagCharacter)
	{
		return; 
	}

	AXYZ_BaseCharacter* Character = StaticCast<AXYZ_BaseCharacter*>(ObjectInfo->GetObject());
	if (!IsValid(Character))
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement(); 
	AAIController* AIController = Character->GetController<AAIController>(); 
	UWidgetComponent* Widget = Character->HealthBarProgressComponent; 

	if (Significance == SignificanceValueVeryHigh)
	{
		MovementComponent->SetComponentTickInterval(0.0f); 
		Widget->SetVisibility(true); 
		Character->GetMesh()->SetComponentTickEnabled(true); 
		Character->GetMesh()->SetComponentTickInterval(0.0f); 
		if (IsValid(AIController))
		{
			AIController->SetActorTickInterval(0.0f); 
		}
	}
	else if (Significance == SignificanceValueHigh)
	{
		MovementComponent->SetComponentTickInterval(0.0f);
		Widget->SetVisibility(true);
		Character->GetMesh()->SetComponentTickEnabled(true);
		Character->GetMesh()->SetComponentTickInterval(0.05f);
		if (IsValid(AIController))
		{
			AIController->SetActorTickInterval(0.0f);
		}
	}
	else if (Significance == SignificanceValueMedium)
	{
		MovementComponent->SetComponentTickInterval(0.1f); //ten times per second
		Widget->SetVisibility(false);
		Character->GetMesh()->SetComponentTickEnabled(true);
		Character->GetMesh()->SetComponentTickInterval(0.1f);
		if (IsValid(AIController))
		{
			AIController->SetActorTickInterval(0.1f);
		}
	}
	else if (Significance == SignificanceValueLow)
	{
		MovementComponent->SetComponentTickInterval(1.0f);
		Widget->SetVisibility(false);
		Character->GetMesh()->SetComponentTickEnabled(true);
		Character->GetMesh()->SetComponentTickInterval(1.0f);
		if (IsValid(AIController))
		{
			AIController->SetActorTickInterval(1.0f);
		}
	}
	else if (Significance == SignificanceValueVeryLow)
	{
		MovementComponent->SetComponentTickInterval(5.0f);
		Widget->SetVisibility(false);
		Character->GetMesh()->SetComponentTickEnabled(false);
		if (IsValid(AIController))
		{
			AIController->SetActorTickInterval(10.0f);
		}
	}


}

void AXYZ_BaseCharacter::UpdateIKSettings(float DeltaSeconds)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, CalculateIKParametersForSocketName(RightFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, CalculateIKParametersForSocketName(LeftFootSocketName), DeltaSeconds, IKInterpSpeed);
	IKPelvisOffset = FMath::FInterpTo(IKPelvisOffset, CalculateIKPelvisOffset(), DeltaSeconds, IKInterpSpeed);
}

float AXYZ_BaseCharacter::CalculateIKParametersForSocketName(const FName& SocketName) const
{
	float Result = 0.0f;

	float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	const FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	const FVector TraceEnd = TraceStart - (CapsuleHalfHeight + IKTraceDistance) * FVector::UpVector;

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);


	//if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult, true))
	const FVector FootSizeBox = FVector(1.f, 15.f, 7.f);
	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceEnd, FootSizeBox, GetMesh()->GetSocketRotation(SocketName), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true))
	{
		float CharacterBottom = TraceStart.Z - CapsuleHalfHeight;
		Result = CharacterBottom - HitResult.Location.Z;
	}

	return Result;
}

float AXYZ_BaseCharacter::CalculateIKPelvisOffset()
{
	return  IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

void AXYZ_BaseCharacter::OnSprintStart_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("AGCBaseCharacter::OnSprintStart_Implementation"));
}

void AXYZ_BaseCharacter::OnSprintEnd_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("AGCBaseCharacter::OnSprintEnd_Implementation"));
}

bool AXYZ_BaseCharacter::CanSprint()
{
	return !(BaseCharacterMovementComponent->IsOutOfStamina() || BaseCharacterMovementComponent->IsCrouching());
}

bool AXYZ_BaseCharacter::CanRestoreStamina()
{
	return !BaseCharacterMovementComponent->IsSprinting();
}

void AXYZ_BaseCharacter::DebugDrawStamina()
{
	if (CurrentStamina < MaxStamina)
	{
		GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
	}
}



void AXYZ_BaseCharacter::InitializeHealthProgress()
{
	UXYZ_AttributeProgressBar* Widget = Cast<UXYZ_AttributeProgressBar>(HealthBarProgressComponent->GetUserWidgetObject());
	if (!IsValid(Widget))
	{
		HealthBarProgressComponent->SetVisibility(false);
		return;
	}

	//BAG
	if (IsPlayerControlled() && IsLocallyControlled())
	{
		HealthBarProgressComponent->SetVisibility(false);
	}

	CharacterAttributesComponent->OnHealthChangedEvent.AddUObject(Widget, &UXYZ_AttributeProgressBar::SetProgressPercentage);
	CharacterAttributesComponent->OnDeathEvent.AddLambda([=]() {HealthBarProgressComponent->SetVisibility(false); });
	Widget->SetProgressPercentage(CharacterAttributesComponent->GetHealthPercent());

}

bool AXYZ_BaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder();
}

void AXYZ_BaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationDuration)
{
}

void AXYZ_BaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0.0f)
	{
		EnableRagdoll();
	}
}

void AXYZ_BaseCharacter::TryChangeSprintState(float DeltaTime)
{
	if (bIsSprintRequested && !BaseCharacterMovementComponent->IsSprinting() && CanSprint())
	{
		BaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
		//OnSprintStart_Implementation();
	}

	if (BaseCharacterMovementComponent->IsSprinting())
	{
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaTime;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
	}

	if (BaseCharacterMovementComponent->IsSprinting() && !(bIsSprintRequested && CanSprint()))
	{
		BaseCharacterMovementComponent->StopSprint();
		OnSprintEnd();
		//OnSprintEnd_Implementation();
	}
}

const FMantlingSettings& AXYZ_BaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}

void AXYZ_BaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}



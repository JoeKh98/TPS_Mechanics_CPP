// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XYZTypes.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/ScriptInterface.h"
#include "Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "SignificanceManager.h"
#include "XYZ_BaseCharacter.generated.h"



//#include "Controllers/XYZ_PlayerController.h"
DECLARE_DELEGATE_OneParam(FOnInteractableObjectFound, FName);



USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UAnimMontage* MantlingMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UAnimMontage* FPMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float AnimationCorrectionXY = 65.0f;
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float AnimationCorrectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MinHeight = 100.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MinHeightStartTime = 0.5f;


};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool)


class IInteractable;
class UWidgetComponent;
class UCharacterAttributesComponent;
class UCharacterEquipmentComponent;
class UBaseCharacterMovementComponent;
class AInteractiveActor;
class UCharacterInventoryComponent;
class UInventoryItem;

UCLASS(Abstract, NotBlueprintable)
class XYZ_LESSON_API AXYZ_BaseCharacter : public ACharacter, public IGenericTeamAgentInterface, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:
	AXYZ_BaseCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override; 
	virtual void EndPlay(const EEndPlayReason::Type Reason) override; 

	//@ ISaveSubsystemInterface
	virtual void OnLevelDeserialized_Implementation() override; 

	//~ISaveSubsystemInterface

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	virtual void PossessedBy(AController* NewController) override;

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual void Turn(float Value) {};
	virtual void LookUp(float Value) {};
	virtual void TurnAtRate(float Value) {};
	virtual void LookUpAtRate(float Value) {};

	virtual void ChangeCrouchState();

	virtual void StartSprint();
	virtual void StopSprint();



	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};
	virtual void Tick(float DeltaTime) override;

	void StartFire();
	void StopFire();

	//Aiming
	void StartAiming();
	void StopAiming();
	FRotator GetAimOffset();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
		void OnStartAiming();
		
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
		void OnStopAiming();

	bool IsAiming() const;

	void Reload();

	void NextItem();
	void PreviousItem();

	void EquipPrimaryItem();

	float GetAimingMovementSpeed() const; 

	FOnAimingStateChanged OnAimingStateChanged;

	UFUNCTION(BlueprintCallable)
	void Mantle(bool bForce = false);

	UPROPERTY(ReplicatedUsing = OnRep_IsMantling)
		bool bIsMantling;

	UFUNCTION()
	void OnRep_IsMantling(bool bWasMantling);

	virtual bool CanJumpInternal_Implementation() const override;

	FORCEINLINE UBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return BaseCharacterMovementComponent; }

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;

	const UCharacterAttributesComponent* GetCharacterAttributesComponent() const; 

	// ~ begin IK settings
	float GetIKRightFootOffset() const { return IKRightFootOffset; }

	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	float GetIKPelvisOffset() const { return IKPelvisOffset; }
	// ~ end IK settings

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	void ClimbLadderUp(float Value); 
	void InteractWithLadder();
	
	const class ALadder* GetAvailableLadder() const;
	
	virtual void Falling() override; 

	virtual void NotifyJumpApex() override;

	virtual void Landed(const FHitResult& Hit) override; 

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	void Interact(); 
	

	bool PickupItem(TWeakObjectPtr<UInventoryItem> ItemToPickup);
	void UseInventory(APlayerController* PlayerController);

	void RestoreFullStamina();
	void AddHealth(float Health);


	//allows us to bind some widgets to objects in world
	UPROPERTY(VisibleDefaultsOnly, Category = "Character | Components")
		UWidgetComponent* HealthBarProgressComponent; 

	void InitializeHealthProgress(); 

	FOnInteractableObjectFound OnInteractableObjectFound; 

	void ConfirmWeaponSelection(); 

	/* IGenericTeamAgentInterface*/
	virtual FGenericTeamId GetGenericTeamId() const override;
	/* IGenericTeamAgentInterface*/

	

protected:

	// ~ begin IK settings
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting")
		FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting")
		FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
		float IKTraceDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|IK Setting", meta = (ClampMin = 0.f, UIMin = 0.f))
		float IKInterpSpeed = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
		float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
		float StaminaRestoreVelocity = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
		float SprintStaminaConsumptionVelocity = 20.0f;

	// ~ end IK settings

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintStart();
	virtual void OnSprintStart_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Character | Movement")
	void OnSprintEnd();
	virtual void OnSprintEnd_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
		float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controls")
		float BaseLookUpRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
		float SprintSpeed = 800.0f;

	virtual bool CanSprint();

	bool CanRestoreStamina();

	void DebugDrawStamina();


	bool CanMantle() const; 
	
	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime);

	class UBaseCharacterMovementComponent* BaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
		class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
		FMantlingSettings HighMantleSettings; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
		FMantlingSettings LowMantleSettings; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float LowMantleMaxHeight = 125.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
		UCharacterAttributesComponent* CharacterAttributesComponent; 

	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
		class UAnimMontage* OnDeathAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
		class UCurveFloat* FallDamageCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Components")
		UCharacterEquipmentComponent* CharacterEquipmentComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Components")
		UCharacterInventoryComponent* CharacterInventoryComponent; 


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
		ETeams Team = ETeams::Enemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | ")
		float LineOfSightDistance = 500.0f; 

	//spec class for storing ptrs to interfaces
	UPROPERTY()
	TScriptInterface<IInteractable> LineOfSightObject; 


	void TraceLineOfSight(); 

	virtual void OnStartAimingInternal();

	virtual void OnStopAimingInternal();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Significance")
		bool bIsSignificanceEnabled = true; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Significance")
		float VeryHighSignificanceDistance = 1000.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Significance")
		float HighSignificanceDistance = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Significance")
		float MediumSignificanceDistance = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Significance")
		float LowSignificanceDistance = 6000.0f;

private:

	float SignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, const FTransform& ViewPoint);
	void PostSignificanceFunction(USignificanceManager::FManagedObjectInfo* ObjectInfo, float OldSignificance, float Significance, bool bFinal);


	void UpdateIKSettings(float DeltaSeconds);	//DONE

	float CalculateIKParametersForSocketName(const FName& SocketName) const;	//DONE

	float CalculateIKPelvisOffset();	//DONE

	float IKLeftFootOffset = 0.0f;	//DONE
	float IKRightFootOffset = 0.0f;	//DONE
	float IKPelvisOffset = 0.0f;	//DONE

	void UpdateStamina(float DeltaTime);	//DONE
	float CurrentStamina = 100.0f;	//DONE

	bool bIsAiming;

	float CurrentAimingMovementSpeed; 

	void TryChangeSprintState(float DeltaTime);	//DONE

	bool bIsSprintRequested = false;

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;  //DONE

	TArray<AInteractiveActor*> AvailableInteractiveActors; //DONE

	void EnableRagdoll(); //DONE

	FVector CurrentFallApex; //DONE

};

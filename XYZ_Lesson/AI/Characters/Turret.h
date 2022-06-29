// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "XYZTypes.h"
#include "Turret.generated.h"

UENUM(BlueprintType)
enum class ETurretState : uint8
{
	Searching, 
	Firing
};


class UWeaponBarellComponent;
UCLASS()
class XYZ_LESSON_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	
	ATurret();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override; 
	virtual void Tick(float DeltaTime) override;
	virtual FVector GetPawnViewLocation() const override;
	virtual FRotator GetViewRotation() const override; 


	void OnCurrentTargetSet();

	UPROPERTY(ReplicatedUsing=OnRep_CurrentTarget)
	AActor* CurrentTarget = nullptr;

protected: 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* TurretBaseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USceneComponent* TurretBarellComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UWeaponBarellComponent* WeaponBarell;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BaseSearchingRotationRate = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BaseFiringInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BarellPitchRotationRate = 60.0f;

	//Minimum angle of rotation 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MinBarellPitchAngle = -30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float MaxBarellPitchAngle = 60.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 1.0f, UIMin = 1.0f))
		float RateOfFire = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float BulletSpreadAngle = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.0f, UIMin = 0.0f))
		float FireDelayTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Team")
		ETeams Team = ETeams::Enemy;


private:

	void SearchingMovement(float DeltaTime);
	void FiringMovement(float DeltaTime);
	void SetCurrentTurretState(ETurretState NewState);
	void MakeShot();

	float GetFireInterval() const; 

	UFUNCTION()
		void OnRep_CurrentTarget();

	ETurretState CurrentTurretState = ETurretState::Searching; 
	 
	FTimerHandle ShotTimer; 

	
};

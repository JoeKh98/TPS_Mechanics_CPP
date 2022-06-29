// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "CharacterAttributesComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class XYZ_LESSON_API UCharacterAttributesComponent : public UActorComponent, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:	

	UCharacterAttributesComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; 

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;
	FOnHealthChanged OnHealthChangedEvent; 

	bool IsAlive() { return Health > 0.0f; }

	float GetHealthPercent() const;

	void AddHealth(float HealthToAdd);

	//@ ISaveSubsystemInterface
	virtual void OnLevelDeserialized_Implementation() override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.0f))
		float MaxHealth = 100.0f; 


private:
	UPROPERTY(ReplicatedUsing=OnRep_Health, SaveGame)
	float Health = 0.0f;

	UFUNCTION()
		void OnRep_Health();

	void OnHealthChanged();

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

	void DebugDrawAttributes(); 

#endif

	UFUNCTION()
		void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);


	TWeakObjectPtr<class AXYZ_BaseCharacter> CachedBaseCharacterOwner;
};

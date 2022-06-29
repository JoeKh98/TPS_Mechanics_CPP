// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributesComponent.h"
#include "../../Character/XYZ_BaseCharacter.h"
#include "../../Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "../../XYZTypes.h"
#include "Net/UnrealNetwork.h"



UCharacterAttributesComponent::UCharacterAttributesComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true); 

}

float UCharacterAttributesComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void UCharacterAttributesComponent::AddHealth(float HealthToAdd)
{
	Health = FMath::Clamp(Health + HealthToAdd, 0.0f, MaxHealth); 
	OnHealthChanged(); 
}

void UCharacterAttributesComponent::OnLevelDeserialized_Implementation()
{
	OnHealthChanged(); 
}


void UCharacterAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(MaxHealth > 0.0f, TEXT("void UCharacterAttributesComponent::BeginPlay() max health cannot be equal to 0"));

	checkf(GetOwner()->IsA<AXYZ_BaseCharacter>(), TEXT("UCharacterAttributesComponent::BeginPlay() UCharacterAttributesComponent can be used only with AXYZ_BaseCharacter"));
	CachedBaseCharacterOwner = StaticCast<AXYZ_BaseCharacter*>(GetOwner());


	Health = MaxHealth;  
	
	if (GetOwner()->HasAuthority())
	{
		CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributesComponent::OnTakeAnyDamage);
	}
}


void UCharacterAttributesComponent::OnRep_Health()
{
	OnHealthChanged();
}

void UCharacterAttributesComponent::OnHealthChanged()
{
	if (OnHealthChangedEvent.IsBound())
	{
		OnHealthChangedEvent.Broadcast(GetHealthPercent()); 
	}

	if (Health <= 0.0f)
	{
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}


#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT

void UCharacterAttributesComponent::DebugDrawAttributes()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();

	if (!DebugSubsystem->IsCategoryEnabled(DebugCategoryCharacterAttributes))
	{
		return;
	}

	FVector TextLocation = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) + 5.0f * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);

}
#endif

void UCharacterAttributesComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributesComponent::OnTakeAnyDamage %s recieved %.2f amount of damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	OnHealthChanged();

	
}

void UCharacterAttributesComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterAttributesComponent, Health);
}

void UCharacterAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif

	
}


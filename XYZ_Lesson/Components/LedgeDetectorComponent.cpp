#include "LedgeDetectorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h" 
#include "../XYZTypes.h"
#include "../Utils/XYZTraceUtils.h"
#include "DrawDebugHelpers.h"
#include "../Character/XYZ_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../XYZGameInstance.h"
#include "../Subsystems/DebugSubsystem.h"

// Called when the game starts
void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ACharacter>(), TEXT(" ULedgeDetectorComponent::BeginPlay() only a Character can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
	
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{

	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());




#if ENABLE_DRAW_DEBUG 
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryLedgeDetection);
#else 
	bool bIsDebugEnabled = false; 
#endif

	float DrawTime = 2.0f;

	float BottomZOffset = 2.0f;
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight() - BottomZOffset) * FVector::UpVector;

	

	//1. Forward Check __________________________________________________________________________________________

	float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float ForwardCheckCapsuleHalfHeight = (MaximumLedgeHeight - MinumumLedgeHeight) * 0.5f;

	FHitResult ForwardCheckHitResult;
	FVector ForwardStartLocation = CharacterBottom + (MinumumLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;


	

	if (!XYZTraceUtils::SweepCapsuleSingleByChannel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}


	//2. Downward Check __________________________________________________________________________________________


	float DownwardSphereTraceRadius = CapsuleComponent->GetScaledCapsuleRadius();
	FHitResult DownwardCheckHitResult;

	

	float DownwardCheckDepthOffset = 10.0f;
	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardSphereTraceRadius;
	FVector DownwardEndLocation(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);

	if (!XYZTraceUtils::SweepSphereSingleByChannel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, DownwardSphereTraceRadius, ECC_Climbing, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}

	//3. Overlap Check __________________________________________________________________________________________

	float OverlapCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	float OverlapCapsuleFloorOffset = 2.0f;
	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;

	if (XYZTraceUtils::OverlapCapsuleBlockingByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QueryParams, bIsDebugEnabled, DrawTime))
	{
		return false; 
	}
	

	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;

	return true;
}
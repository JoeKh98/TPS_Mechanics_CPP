#pragma once

namespace XYZTraceUtils
{
	//TODO
	bool SweepCapsuleSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false, float DrawTime = -1, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);

	bool SweepSphereSingleByChannel(const UWorld* World, struct FHitResult& OutHit, const FVector& Start, const FVector& End, float SphereRadius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params = FCollisionQueryParams::DefaultQueryParam, const FCollisionResponseParams& ResponseParam = FCollisionResponseParams::DefaultResponseParam, bool bDrawDebug = false, float DrawTime = -1, FColor TraceColor = FColor::Black, FColor HitColor = FColor::Red);
	
	bool OverlapCapsuleAnyByProfile(const UWorld* World, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rot, FName ProfileName, const FCollisionQueryParams& QueryParams, bool bDrawDebug = false, float DrawTime = -1, FColor HitColor = FColor::Red);

	bool OverlapCapsuleBlockingByProfile(const UWorld* World, const FVector& Pos, float CapsuleRadius, float CapsuleHalfHeight, FQuat Rot, FName ProfileName, const FCollisionQueryParams& QueryParams, bool bDrawDebug = false, float DrawTime = -1, FColor HitColor = FColor::Red);
}

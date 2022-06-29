#pragma once

#define ECC_Climbing ECC_GameTraceChannel1
#define ECC_InteractionVolume ECC_GameTraceChannel2
#define ECC_Bullet ECC_GameTraceChannel3
#define ECC_Melee ECC_GameTraceChannel4

const FName FXParamTraceEnd = FName("TraceEnd");

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryCharacterAttributes = FName("CharacterAttributes");
const FName DebugCategoryRangeWeapon = FName("RangeWeapon");
const FName DebugCategoryMeleeWeapon = FName("MeleeWeapon");

const FName SocketFPCamera = FName("CameraSocket");
const FName SocketCharacterWeapon = FName("CharacterWeaponSocket");
const FName SocketCharacterThrowable = FName("ThrowableSocket");
const FName SocketWeaponMuzzle = FName("MuzzleSocket");
const FName SocketWeaponForeGrip = FName("ForeGripSocket");

const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");
const FName CollisionProfilePawn = FName("Pawn");
const FName CollisionProfileNoCollision = FName("NoCollision");
const FName CollisionProfileRagdoll = FName("Ragdoll");


const FName SectionMontageReloadEnd = FName("ReloadEnd");

const FName BB_CurrentTarget = FName("CurrentTarget");
const FName BB_NextLocation = FName("NextLocation");

const FName ActionInteract = FName("Interact"); 


const FName SignificanceTagCharacter = FName("Character"); 

const float SignificanceValueVeryHigh = 0.0f; 
const float SignificanceValueHigh = 1.0f; 
const float SignificanceValueMedium = 10.0f; 
const float SignificanceValueLow = 100.0f;
const float SignificanceValueVeryLow = 1000.0f;






UENUM(BlueprintType)
enum class EEquipableItemType : uint8
{
	None,
	Pistol, 
	Rifle, 
	Throwable,
	Melee
};

UENUM(BlueprintType)
enum class EAmunitionType : uint8
{
	None,
	Pistol, 
	Rifle,
	ShotgunShells,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlots : uint8
{
	 None,
	 SideArm,
	 PrimaryWeapon,
	 SecondaryWeapon,
	 PrimaryItemSlot,
	 MeleeWeapon,
	 MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReticleType : uint8
{
	None, 
	Default,
	SniperRifle,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EMeleeAttackTypes : uint8
{
	None, 
	PrimaryAttack,
	SecondaryAttack,
	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETeams : uint8
{
	Player, 
	Enemy
};
// Copyright Epic Games, Inc. All Rights Reserved.

#include "XYZ_LessonCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "WheeledVehicle.h"
#include "Kismet/GameplayStatics.h"
#include "XYZ_LessonGameInstance.h "


//////////////////////////////////////////////////////////////////////////
// AXYZ_LessonCharacter

AXYZ_LessonCharacter::AXYZ_LessonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AXYZ_LessonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AXYZ_LessonCharacter::Fire);
	PlayerInputComponent->BindAction("SitInACar", IE_Released, this, &AXYZ_LessonCharacter::SitInACar);
	PlayerInputComponent->BindAction("LeaveACar", IE_Released, this, &AXYZ_LessonCharacter::LeaveACar);

	PlayerInputComponent->BindAxis("MoveForward", this, &AXYZ_LessonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AXYZ_LessonCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AXYZ_LessonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AXYZ_LessonCharacter::LookUpAtRate);



	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AXYZ_LessonCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AXYZ_LessonCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AXYZ_LessonCharacter::OnResetVR);
}


void AXYZ_LessonCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}


//fffffffffffffffffffffffffffffffffffffffffffff
//fffffffffffffffffffffffffffffffffffffffffffff
//fffffffffffffffffffffffffffffffffffffffffffff
//fffffffffffffffffffffffffffffffffffffffffffff
//fffffffffffffffffffffffffffffffffffffffffffff

void AXYZ_LessonCharacter::BeginPlay()
{
	Super::BeginPlay();
	UMaterialInstanceDynamic* NewMaterialInstance = GetMesh()->CreateDynamicMaterialInstance(0);
	UXYZ_LessonGameInstance* GameInstance = Cast<UXYZ_LessonGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (IsValid(GameInstance) && IsValid(NewMaterialInstance))
	{
		NewMaterialInstance->SetVectorParameterValue(FName("BodyColor"), FLinearColor(GameInstance->GetPlayerColor()));
	}


}

void AXYZ_LessonCharacter::Fire()
{
	if (!IsValid(ProjectileType))
	{
		return;
	}

	FRotator SpawningRotation = GetActorRotation();
	FVector SpawningLocation = GetActorLocation() + SpawningRotation.RotateVector(FiringOffset);
	AActor* SpawningActor = GetWorld()->SpawnActor(ProjectileType, &SpawningLocation, &SpawningRotation);
	UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(SpawningActor->GetRootComponent());


	if (IsValid(RootComponent))
	{
		RootPrimitive->AddImpulse(RootComponent->GetForwardVector() * FireVelocity, NAME_None, true);
	}


}




void AXYZ_LessonCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AXYZ_LessonCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AXYZ_LessonCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AXYZ_LessonCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AXYZ_LessonCharacter::SitInACar()
{
	CurrentController = GetController();
	if (IsValid(CurrentActiveCar))
	{
		CurrentController->Possess(CurrentActiveCar);
		InACar = true;
		AttachToActor(CurrentActiveCar, FAttachmentTransformRules::KeepRelativeTransform);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorRelativeLocation(FVector(0.0f, -200.0f, 100.0f));
		GetMesh()->SetVisibility(false);

	}

	
}

void AXYZ_LessonCharacter::LeaveACar()
{
	//AController* CurrentController = GetController();
	if (InACar == true)
	{
		SetActorRelativeLocation(FVector(100.0f, -200.0f, 0.0f));
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		
		GetMesh()->SetVisibility(true);
		CurrentController->Possess(this);
		InACar = false;
	}
	
	

}

void AXYZ_LessonCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AXYZ_LessonCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

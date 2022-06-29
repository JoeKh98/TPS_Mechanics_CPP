// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "Components/TimelineComponent.h"
#include "Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
#include "Door.generated.h"

UCLASS()
class XYZ_LESSON_API ADoor : public AActor, public IInteractable, public ISaveSubsystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Interact(AXYZ_BaseCharacter* Character) override; 
	virtual FName GetActionEventName() const override; 

	virtual bool HasOnInteractionCallback() const ; 
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override; 
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DelegateHandle) override; 

	//@ ISaveSubsystemInterface
	virtual void OnLevelDeserialized_Implementation() override;

	//~ISaveSubsystemInterface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door" )
		UStaticMeshComponent* DoorMesh; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
		USceneComponent* DoorPivot; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
		float AngleClosed = 0.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
		float AngleOpened = 90.0f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
		UCurveFloat* DoorAnimationCurve; 

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	IInteractable::FOnInteraction OnInteractionEvent; 

private: 
	void InteractWithDoor(); 

	UFUNCTION()
		void UpdateDoorAnim(float Alpha);

	UFUNCTION()
		void OnDoorAnimationFinished(); 

	FTimeline DoorOpenAnimTimeline; 

	UPROPERTY(SaveGame)
	bool bIsOpened = false; 
};

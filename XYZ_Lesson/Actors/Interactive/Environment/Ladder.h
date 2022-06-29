// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Ladder.generated.h"

class UAnimMontage;
class UBoxComponent; 
class UStaticMeshComponent; 
/**
 * 
 */
UCLASS(Blueprintable)
class XYZ_LESSON_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()

public:
	ALadder();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	float GetLadderHeight() const;

	UAnimMontage* GetAttachFromTopAnimMontage() const;

	FVector GetAttachTopAnimMontageStartingLocation() const;
	

	bool GetIsOnTop() const; 

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float LadderHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float LadderWidth = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float StepsInterval = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		float BottomStepOffset = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		UAnimMontage* AttachFromTopAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
		FVector AttachFromTopAnimMontageInitialOffset = FVector::ZeroVector;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* RightRailMeshComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UInstancedStaticMeshComponent* StepsMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* TopInteractionVolume;

	UBoxComponent* GetLadderInteractionBox() const;

	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


private: 

	bool bIsOnTop = false; 
};

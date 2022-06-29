// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "DoorActor.generated.h"

UCLASS()
class XYZ_LESSON_API ADoorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Begin and End	Overlap Events for our DoorProxVolume
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UTimelineComponent* DoorTimelineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* DoorProxVolume;
		
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//variable to hold the Curve asset
	UPROPERTY(EditAnywhere)
		UCurveFloat* DoorTimelineFloatCurve;

private:

	//Float Track Signature to handle our update track event
	FOnTimelineFloat UpdateFunctionFloat;

	//Function which update our Door's relative location with Timeline graph 
	UFUNCTION()
		void UpdateTimelineComp(float Alpha);





};

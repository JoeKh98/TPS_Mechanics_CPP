// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorActor.h"
#include "Components/BoxComponent.h"


// Sets default values
ADoorActor::ADoorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	//Create our default components
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrameMesh"));
	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorTimelineComp = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimelineComp"));
	DoorProxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorProximityComponent")); 

	//Setup Attachments
	DoorFrame->SetupAttachment(RootComponent);
	Door->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);
	DoorProxVolume->AttachToComponent(DoorFrame, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void ADoorActor::BeginPlay()
{
	Super::BeginPlay();

	//Binding our float track to our UpdateTimelineComp Functions output
	UpdateFunctionFloat.BindDynamic(this, &ADoorActor::UpdateTimelineComp);

	//If we have float curve, bind it's graph to our update function 
	if (IsValid(DoorTimelineFloatCurve))
	{
		DoorTimelineComp->AddInterpFloat(DoorTimelineFloatCurve, UpdateFunctionFloat);
	}

	DoorProxVolume->OnComponentBeginOverlap.AddDynamic(this, &ADoorActor::OnOverlapBegin);
	DoorProxVolume->OnComponentEndOverlap.AddDynamic(this, &ADoorActor::OnOverlapEnd);
	
}

void ADoorActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DoorTimelineComp->Play();  
}

void ADoorActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	DoorTimelineComp->Reverse();
}

// Called every frame
void ADoorActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoorActor::UpdateTimelineComp(float Alpha)
{

	//Create and set our Door's new relative location based on the output from our Timeline Curve

	FRotator DoorNewRotation = FRotator(0.0f, Alpha, 0.0f);

	Door->SetRelativeRotation(DoorNewRotation);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacterSpawner.h"
#include "AI/Characters/XYZ_AICharacter.h"
#include "Actors/Interactive/Interface/Interactive.h"
#include "UObject/ScriptInterface.h"

// Sets default values
AAICharacterSpawner::AAICharacterSpawner()
{

	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnRoot"));
	SetRootComponent(SceneRoot);


}

void AAICharacterSpawner::SpawnAI()
{
	if (!bCanSpawn || !IsValid(CharacterClass))
	{
		return; 
	}

	

	AXYZ_AICharacter* AICharacter = GetWorld()->SpawnActor<AXYZ_AICharacter>(CharacterClass, GetTransform()); 
	//spawn in place where current actor is situated

	if (!IsValid(AICharacter->Controller))
	{
		AICharacter->SpawnDefaultController();
	}

	if (bDoOnce)
	{
		UnSubscribeFromTrigger();
		bCanSpawn = false;
	}

}

void AAICharacterSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{

	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(AAICharacterSpawner, SpawnTriggerActor))
	{
		SpawnTrigger = SpawnTriggerActor; 
		if (SpawnTrigger.GetInterface())
		{
			if (!SpawnTrigger->HasOnInteractionCallback())
			{
				SpawnTriggerActor = nullptr; 
				SpawnTrigger = nullptr; 
			}
		}
		else
		{
			SpawnTriggerActor = nullptr;
			SpawnTrigger = nullptr;
		}

	}


}

// Called when the game starts or when spawned
void AAICharacterSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (SpawnTrigger.GetInterface())
	{
		TriggerHandle = SpawnTrigger->AddOnInteractionUFunction(this, FName("SpawnAI"));
	}

	if (bIsSpawnOnStart)
	{
		SpawnAI();
	}
	
}

void AAICharacterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeFromTrigger(); 
	Super::EndPlay(EndPlayReason);

}

void AAICharacterSpawner::UnSubscribeFromTrigger()
{
	if (TriggerHandle.IsValid() && SpawnTrigger.GetInterface())
	{
		SpawnTrigger->RemoveOnInteractionDelegate(TriggerHandle);
	}
}




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "MyAnimNotify_AttachEquippedItem.generated.h"


/**
 * 
 */
UCLASS()
class XYZ_LESSON_API UMyAnimNotify_AttachEquippedItem : public UAnimNotify
{
	GENERATED_BODY()

		virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override; 

	
};

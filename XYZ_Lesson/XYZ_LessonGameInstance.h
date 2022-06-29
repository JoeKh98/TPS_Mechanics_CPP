// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "XYZ_LessonGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class XYZ_LESSON_API UXYZ_LessonGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	const FColor& GetPlayerColor();

private:

	// black by default 
	FColor PlayerColor = FColor::Black; 

};

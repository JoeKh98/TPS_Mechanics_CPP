// Copyright Epic Games, Inc. All Rights Reserved.

#include "XYZ_LessonGameMode.h"
#include "XYZ_LessonCharacter.h"
#include "UObject/ConstructorHelpers.h"

AXYZ_LessonGameMode::AXYZ_LessonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

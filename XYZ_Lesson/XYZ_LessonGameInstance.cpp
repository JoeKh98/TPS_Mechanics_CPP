// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_LessonGameInstance.h"

const FColor& UXYZ_LessonGameInstance::GetPlayerColor()
{
	while (PlayerColor == FColor::Black)
	{
		PlayerColor = FColor::MakeRandomColor();

	}

	return PlayerColor; 
}
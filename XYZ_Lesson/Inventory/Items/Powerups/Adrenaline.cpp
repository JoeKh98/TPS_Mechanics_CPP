// Fill out your copyright notice in the Description page of Project Settings.


#include "Adrenaline.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h "
#include "Character/XYZ_BaseCharacter.h"

bool UAdrenaline::Consume(AXYZ_BaseCharacter* ConsumeTarget)
{
	ConsumeTarget->RestoreFullStamina(); 
	this->ConditionalBeginDestroy(); 
	return true; 
}
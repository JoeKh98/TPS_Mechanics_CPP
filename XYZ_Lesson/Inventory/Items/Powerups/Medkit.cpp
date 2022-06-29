// Fill out your copyright notice in the Description page of Project Settings.


#include "Medkit.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h "
#include "Character/XYZ_BaseCharacter.h"

bool UMedkit::Consume(AXYZ_BaseCharacter* ConsumeTarget)
{
	ConsumeTarget->AddHealth(Health);
	this->ConditionalBeginDestroy();
	return true; 

}
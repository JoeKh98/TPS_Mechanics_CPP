// Fill out your copyright notice in the Description page of Project Settings.


#include "XYZ_AttributeProgressBar.h"
#include "Components/ProgressBar.h"

void UXYZ_AttributeProgressBar::SetProgressPercentage(float Percentage)
{
	HealthProgressBar->SetPercent(Percentage);
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XYZ_AttributeProgressBar.generated.h"

/**
 * 
 */

class UProgressBar; 

UCLASS()
class XYZ_LESSON_API UXYZ_AttributeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public: 
	void SetProgressPercentage(float Percentage); 

protected: 
	
	//allows from cpp bind to some widgets that we create in BP
	UPROPERTY(meta = (BindWidget))
		UProgressBar* HealthProgressBar; 

	
};

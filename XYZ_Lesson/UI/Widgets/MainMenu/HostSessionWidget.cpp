// Fill out your copyright notice in the Description page of Project Settings.


#include "HostSessionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "XYZGameInstance.h"

void UHostSessionWidget::CreateSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UXYZGameInstance>());
	UXYZGameInstance* XYZGameInstance = StaticCast<UXYZGameInstance*>(GetGameInstance());

	XYZGameInstance->LaunchLobby(4, ServerName, bIsLAN); 

}

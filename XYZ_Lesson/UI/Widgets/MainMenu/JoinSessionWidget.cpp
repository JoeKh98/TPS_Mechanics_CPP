// Fill out your copyright notice in the Description page of Project Settings.


#include "JoinSessionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "XYZGameInstance.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UXYZGameInstance>());
	XYZGameInstance = StaticCast<UXYZGameInstance*>(GetGameInstance());


}

void UJoinSessionWidget::FindOnlineSession()
{
	XYZGameInstance->OnMatchFound.AddUFunction(this, FName("OnMatchFound"));
	XYZGameInstance->FindAMatch(bIsLAN);
	SearchingSessionState = ESearchingSessionState::Searching; 
}

void UJoinSessionWidget::JoinOnlineSession()
{
	XYZGameInstance->JoinOnlineGame();
}

void UJoinSessionWidget::OnMatchFound_Implementation(bool bIsSuccessful)
{
	SearchingSessionState = bIsSuccessful ? ESearchingSessionState::SessionIsFound : ESearchingSessionState::None;

	XYZGameInstance->OnMatchFound.RemoveAll(this); 

}

void UJoinSessionWidget::CloseWidget()
{
	XYZGameInstance->OnMatchFound.RemoveAll(this);
	Super::CloseWidget(); 
}
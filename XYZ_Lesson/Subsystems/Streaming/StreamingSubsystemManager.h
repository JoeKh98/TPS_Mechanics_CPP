#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/LevelStreaming.h"
#include "StreamingSubsystemManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStreamingSubsystem, Log, All);

class AStreamingSubsystemVolume;
UCLASS()
class XYZ_LESSON_API UStreamingSubsystemManager : public UObject
{
	GENERATED_BODY()
	
public:
	UStreamingSubsystemManager(); 

	void Initialize(ULevelStreaming* InStreamingLevel, const FString& InLevelName);
	void Deinitialize();

	void AddLoadRequest(AStreamingSubsystemVolume* SubsystemVolume);
	void RemoveLoadRequest(AStreamingSubsystemVolume* SubsystemVolume);

	void AddUnloadRequest(AStreamingSubsystemVolume* SubsystemVolume);
	void RemoveUnloadRequest(AStreamingSubsystemVolume* SubsystemVolume);

	ULevelStreaming::ECurrentState GetStreamingLevelState() const;

private:
	UFUNCTION()
		void OnLevelLoaded();

	UFUNCTION()
		void OnLevelUnloaded();

	void LoadLevel();
	void UnloadLevel();

	FString LevelName;
	ULevelStreaming::ECurrentState StreamingLevelState;
	TWeakObjectPtr<ULevelStreaming> StreamingLevel;
	TArray<TWeakObjectPtr<AStreamingSubsystemVolume>> LoadRequests;
	TArray<TWeakObjectPtr<AStreamingSubsystemVolume>> UnloadRequests;
};

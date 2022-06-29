// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class XYZ_Lesson : ModuleRules
{
	public XYZ_Lesson(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay", 
			"PhysXVehicles", 
			"Niagara", 
			"UMG", 
			"GameplayTasks", 
			"NavigationSystem",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"SignificanceManager"
		});

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");

		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		PrivateIncludePaths.AddRange(new string[] { Name });
	}
}

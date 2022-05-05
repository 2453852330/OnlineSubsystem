// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Net : ModuleRules
{
	public Net(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay","OnlineSubsystem","OnlineSubsystemUtils" });
	}
}

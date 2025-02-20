// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Progress : ModuleRules
{
	public Progress(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[] {
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
            "ModularGameplay",
            "EnhancedInput",
        });


		PrivateDependencyModuleNames.AddRange(
			new string[] {  
			
			});
	}
}

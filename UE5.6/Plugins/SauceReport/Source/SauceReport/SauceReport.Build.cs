// Copyright Sauce Labs, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SauceReport : ModuleRules
{
	public SauceReport(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"DeveloperSettings",
				"UMG"
			});

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"EditorFramework",
				"DeveloperSettings",
				"DeveloperToolSettings",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			});
	}
}

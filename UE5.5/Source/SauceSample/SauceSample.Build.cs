// Sauce Labs Inc. All Rights Reserved.

using UnrealBuildTool;

public class SauceSample : ModuleRules
{
	public SauceSample(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "SlateCore", "Slate"
		});
	}
}

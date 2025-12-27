// Sauce Labs Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SauceSampleTarget : TargetRules
{
	public SauceSampleTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.AddRange( new string[] { "SauceSample" } );
	}
}

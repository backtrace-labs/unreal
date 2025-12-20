// Sauce Labs Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SauceSampleEditorTarget : TargetRules
{
	public SauceSampleEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;

		ExtraModuleNames.AddRange( new string[] { "SauceSample" } );
	}
}

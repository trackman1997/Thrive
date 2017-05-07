// Copyright (C) 2013-2017  Revolutionary Games

using UnrealBuildTool;
using System.Collections.Generic;

public class ThriveEditorTarget : TargetRules
{
	public ThriveEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "Thrive" } );
	}
}

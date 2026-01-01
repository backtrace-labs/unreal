// Copyright Sauce Labs, Inc. All Rights Reserved.

#include "SauceReportCommands.h"

#define LOCTEXT_NAMESPACE "FSauceReportModule"

void FSauceReportCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Error Reporting", "SauceLabs Error Reporting", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

// Copyright Sauce Labs, Inc. All Rights Reserved.

#include "SauceReportCommands.h"

#define LOCTEXT_NAMESPACE "FSauceReportModule"

void FSauceReportCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "SauceReport", "Sauce Report Checklist", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

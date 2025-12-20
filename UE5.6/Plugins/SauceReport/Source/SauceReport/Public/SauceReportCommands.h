// Copyright Sauce Labs, Inc. All Rights Reserved.

#pragma once

#include <Framework/Commands/Commands.h>

#include "SauceReportStyle.h"

class FSauceReportCommands : public TCommands<FSauceReportCommands>
{
public:

	FSauceReportCommands()
		: TCommands<FSauceReportCommands>(TEXT("SauceReport"), NSLOCTEXT("Contexts", "SauceReport", "SauceReport Plugin"), NAME_None, FSauceReportStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;


	TSharedPtr<FUICommandInfo> PluginAction{nullptr};
};

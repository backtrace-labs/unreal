// Copyright Sauce Labs, Inc. All Rights Reserved.

#pragma once

#include <Modules/ModuleManager.h>

DECLARE_LOG_CATEGORY_EXTERN(LogSauceReport, Log, All);

class FToolBarBuilder;
class FMenuBuilder;

class FSauceReportModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** Bound to the Command */
	void PluginButtonClicked();
	
private:
	void RegisterSettings();
	void RegisterMenus();

	void UnregisterSettings();
	bool HandlePluginUserSettingsSaved();
	void HandleCustomSettingsSaved(UObject*, struct FPropertyChangedEvent&);

	TSharedPtr<class FUICommandList> PluginCommands{nullptr};
};

// Copyright Sauce Labs, Inc. All Rights Reserved.

#pragma once

#include <Modules/ModuleManager.h>

DECLARE_LOG_CATEGORY_EXTERN(LogSauceReport, Log, All);

class FMenuBuilder;
class FSpawnTabArgs;
class FToolBarBuilder;
class SDockTab;

class FSauceReportModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterSettings();
	void RegisterMenus();
	void RegisterSauceReportTab();

	void UnregisterSettings();
	void UnregisterSauceReportTab();
	bool HandlePluginUserSettingsSaved();
	void HandleCustomSettingsSaved(UObject*, struct FPropertyChangedEvent&);

	TSharedRef<SDockTab> SpawnSauceReportTab(const FSpawnTabArgs& SpawnTabArgs);
	// Bound to the plugin command
	void InvokeSauceReportTab();
	TSharedPtr<class FUICommandList> PluginCommands{nullptr};
};

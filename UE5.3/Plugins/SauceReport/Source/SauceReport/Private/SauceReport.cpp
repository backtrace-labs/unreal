// Copyright Sauce Labs, Inc. All Rights Reserved.

#include "SauceReport.h"

#include <ISettingsModule.h>
#include <ISettingsSection.h>
#include <ToolMenus.h>
#include <Framework/Docking/TabManager.h>
#include <Misc/DataDrivenPlatformInfoRegistry.h>

#include "SauceReportChecklist.h"
#include "SauceReportCommands.h"
#include "SauceReportSettings.h"
#include "SauceReportStyle.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FSauceReportModule"

DEFINE_LOG_CATEGORY(LogSauceReport);

static const FName SauceReportTabName(TEXT("SauceReportTab"));

void FSauceReportModule::StartupModule()
{
	// This code will execute after the module is loaded into memory; 
	// the exact timing is specified in the .uplugin file per-module (LoadingPhase)
	FSauceReportStyle::Initialize();
	FSauceReportStyle::ReloadTextures();

	FSauceReportCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(FSauceReportCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSauceReportModule::InvokeSauceReportTab),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSauceReportModule::RegisterMenus));
	RegisterSettings();
	RegisterSauceReportTab();
}

void FSauceReportModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UnregisterSettings();
	UnregisterSauceReportTab();

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSauceReportStyle::Shutdown();

	FSauceReportCommands::Unregister();
}

void FSauceReportModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", 
			"Plugins", "SauceReportSettings",
			LOCTEXT("RuntimeGeneralSettingsName", "Sauce Labs Error Reporting"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Configure Crash Report Settings"),
			GetMutableDefault<USauceReportSettings>());

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FSauceReportModule::HandlePluginUserSettingsSaved);
		}
	}
}

void FSauceReportModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("MainFrame.MainMenu.Tools");
		{
			FToolMenuSection* Section = Menu->Sections.FindByPredicate([](const FToolMenuSection& TMS){ return TMS.Name.IsEqual(TEXT("CrashReporter")); });
			if (!Section)
			{
				Section = &Menu->AddSection("CrashReporter", FText::FromString(TEXT("Crash Reporter")));
			}

			Section->AddMenuEntryWithCommandList(FSauceReportCommands::Get().PluginAction, PluginCommands);
		}
	}

	// TODO: This would add a button in the tool bar 
	/*{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSauceReportCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}*/
}

void FSauceReportModule::RegisterSauceReportTab()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SauceReportTabName,
		FOnSpawnTab::CreateRaw(this,&FSauceReportModule::SpawnSauceReportTab))
		.SetDisplayName(LOCTEXT("TabTitle", "Configure Error Reporting"))
		.SetTooltipText(LOCTEXT("TabTooltip", "Open the Sauce Labs Error Reporting configuration"))
		.SetIcon(FSlateIcon(FSauceReportStyle::GetStyleSetName(), "SauceReport.PluginAction"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSauceReportModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "My Custom Settings");
	}
}

void FSauceReportModule::UnregisterSauceReportTab()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SauceReportTabName);
}

bool FSauceReportModule::HandlePluginUserSettingsSaved()
{
	const USauceReportSettings* Settings = GetDefault<USauceReportSettings>();
	// 1. Handle DefaultEngine.ini
	const FString DefaultEngineIni = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	if (!SauceLabs::UpdateSectionKeyValueOnIniFile(DefaultEngineIni, TEXT("CrashReportClient"), 
		TEXT("DataRouterUrl"), Settings->DefaultDataRouterUrl))
	{
		return false;
	}

	// 2. Handle Platform specific INIs
	const TMap<FName, FDataDrivenPlatformInfo>& AllPlatformInfos = FDataDrivenPlatformInfoRegistry::GetAllPlatformInfos();
	const FString PlatformNames = SauceLabs::PlatformNamesToString(AllPlatformInfos);
	for (const TTuple<FString, FString>& PlatUrl : Settings->PlatformsToDataRouterUrl)
	{
		bool bFound = false;
		FString PlatformName(PlatUrl.Key);
		for (const TTuple<FName, FDataDrivenPlatformInfo>& PlatformInfo : AllPlatformInfos)
		{
			if (PlatformInfo.Key.IsEqual(*PlatformName, ENameCase::IgnoreCase))
			{
				PlatformName = PlatformInfo.Key.ToString(); // Make sure we use the same case-sensitive
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			UE_LOG(LogSauceReport, Warning, TEXT("Failed to find Platform %s, valid platform names: %s"), *PlatformName, *PlatformNames);
			continue;
		}

		if (SauceLabs::EnsureEnginePlatformIniExists(PlatUrl.Key))
		{
			const FString PlatformIniPath = SauceLabs::GetProjectPlatformEngineIniPath(PlatUrl.Key);
			if (!SauceLabs::UpdateSectionKeyValueOnIniFile(PlatformIniPath, TEXT("CrashReportClient"), 
				TEXT("DataRouterUrl"), PlatUrl.Value))
			{
				return false;
			}
		}
	}

	return true;
}

void FSauceReportModule::HandleCustomSettingsSaved(UObject*, FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet)
	{
		HandlePluginUserSettingsSaved();
	}
}

TSharedRef<SDockTab> FSauceReportModule::SpawnSauceReportTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
	.TabRole(ETabRole::NomadTab)
	[
		SNew(SSauceReportChecklist)
	];
}

void FSauceReportModule::InvokeSauceReportTab()
{
	FGlobalTabmanager::Get()->TryInvokeTab(SauceReportTabName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSauceReportModule, SauceReport)
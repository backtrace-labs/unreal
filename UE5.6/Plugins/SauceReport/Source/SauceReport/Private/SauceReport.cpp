// Copyright Sauce Labs, Inc. All Rights Reserved.

#include "SauceReport.h"

#include <ISettingsModule.h>
#include <ISettingsSection.h>
#include <ToolMenus.h>
#include <Misc/DataDrivenPlatformInfoRegistry.h>
#include <Misc/MessageDialog.h>

#include "SauceReportCommands.h"
#include "SauceReportSettings.h"
#include "SauceReportStyle.h"

#define LOCTEXT_NAMESPACE "FSauceReportModule"

DEFINE_LOG_CATEGORY(LogSauceReport);

void FSauceReportModule::StartupModule()
{
	// This code will execute after the module is loaded into memory; 
	// the exact timing is specified in the .uplugin file per-module (LoadingPhase)
	RegisterSettings();
	FSauceReportStyle::Initialize();
	FSauceReportStyle::ReloadTextures();

	FSauceReportCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FSauceReportCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FSauceReportModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSauceReportModule::RegisterMenus));
}

void FSauceReportModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FSauceReportStyle::Shutdown();

	FSauceReportCommands::Unregister();
}

void FSauceReportModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
		LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
		FText::FromString(TEXT("FSauceReportModule::PluginButtonClicked()")),
		FText::FromString(TEXT("SauceReport.cpp")));
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

void FSauceReportModule::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", 
			"Plugins", "SauceReportSettings",
			LOCTEXT("RuntimeGeneralSettingsName", "Sauce Report Settings"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Crash Report Settings"),
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
			FToolMenuSection& Section = Menu->FindOrAddSection("Instrumentation");
			Section.AddMenuEntryWithCommandList(FSauceReportCommands::Get().PluginAction, PluginCommands);
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

void FSauceReportModule::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "My Custom Settings");
	}
}

bool FSauceReportModule::HandlePluginUserSettingsSaved()
{
	const USauceReportSettings* Settings = GetDefault<USauceReportSettings>();
	// 1. Handle DefaultEngine.ini
	const FString DefaultEngineIni = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	SauceLabs::UpdateSpecificIniFile(DefaultEngineIni, TEXT("CrashReportClient"), TEXT("DataRouterUrl"), Settings->DefaultDataRouterUrl);

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
			SauceLabs::UpdateSpecificIniFile(PlatformIniPath, TEXT("CrashReportClient"), TEXT("DataRouterUrl"), PlatUrl.Value);
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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSauceReportModule, SauceReport)
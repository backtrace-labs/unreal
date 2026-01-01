// Copyright Sauce Labs, Inc. All Rights Reserved.

#include "SauceReportChecklist.h"

#include <DesktopPlatformModule.h>
#include <ISettingsModule.h>
#include <Framework/Application/SlateApplication.h>
#include <HAL/PlatformFileManager.h>
#include <Misc/ConfigCacheIni.h>
#include <Misc/ConfigContext.h>
#include <Misc/DataDrivenPlatformInfoRegistry.h>
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>
#include <Settings/ProjectPackagingSettings.h>
#include <Styling/AppStyle.h>
#include <Widgets/Images/SImage.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SBorder.h>
#include <Widgets/Layout/SBox.h>
#include <Widgets/Layout/SExpandableArea.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Layout/SSeparator.h>
#include <Widgets/Layout/SSpacer.h>
#include <Widgets/Text/STextBlock.h>

#include "SauceReport.h"
#include "SauceReportSettings.h"
#include "SauceReportStyle.h"

#define LOCTEXT_NAMESPACE "SauceReportChecklist"

void SSauceReportChecklist::Construct(const FArguments& InArgs)
{
	// Gather available platforms
	const TMap<FName, FDataDrivenPlatformInfo>& AllPlatformInfos = FDataDrivenPlatformInfoRegistry::GetAllPlatformInfos();
	for (const auto& PlatformInfo : AllPlatformInfos)
	{
		if (!PlatformInfo.Key.IsNone() && !PlatformInfo.Key.ToString().IsEmpty())
		{
			int32 PlatformIndex = AvailablePlatforms.Add(PlatformInfo.Key.ToString());
			if (!PlatformInfo.Value.bCanUseCrashReporter)
			{
				AvailablePlatforms[PlatformIndex] += TEXT(" (No Crash Reporter)");
			}
		}
	}
	AvailablePlatforms.Sort();

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(FMargin(12.0f))
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SAssignNew(MainContainer, SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 16)
				[
					BuildHeaderSection()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 8)
				[
					BuildWarningsSection()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 12)
				[
					BuildDefaultEngineSection()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 12)
				[
					BuildPlatformsSection()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 12)
				[
					BuildCrashReporterSection()
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					BuildActionsSection()
				]
			]
		]
	];
}

void SSauceReportChecklist::RefreshChecklist()
{
	if (MainContainer.IsValid())
	{
		MainContainer->ClearChildren();

		MainContainer->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 16)
		[
			BuildHeaderSection()
		];

		MainContainer->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 8)
		[
			BuildWarningsSection()
		];

		MainContainer->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 12)
		[
			BuildDefaultEngineSection()
		];

		MainContainer->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 12)
		[
			BuildPlatformsSection()
		];

		MainContainer->AddSlot()
		.AutoHeight()
		.Padding(0, 0, 0, 12)
		[
			BuildCrashReporterSection()
		];

		MainContainer->AddSlot()
		.AutoHeight()
		[
			BuildActionsSection()
		];
	}
}

bool SSauceReportChecklist::IsDefaultEngineIniWritable()
{
	const FString DefaultEngineIni = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
	return !FPlatformFileManager::Get().GetPlatformFile().IsReadOnly(*DefaultEngineIni);
}

bool SSauceReportChecklist::HasDefaultDataRouterUrl()
{
	return !GetDefaultDataRouterUrl().IsEmpty();
}

FString SSauceReportChecklist::GetDefaultDataRouterUrl()
{
	FString Value;
	const FString DefaultEngine(FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini"));
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*DefaultEngine))
	{
		return Value;
	}

	FConfigFile ConfigFile;
	ConfigFile.Read(DefaultEngine);
	if (const FConfigSection* Section = ConfigFile.FindSection(TEXT("CrashReportClient")))
	{
		if (const FConfigValue* ConfigValue = Section->Find(TEXT("DataRouterUrl")))
		{
			Value = ConfigValue->GetValue();
		}
	}

	return Value;
}

bool SSauceReportChecklist::IsPlatformIniWritable(const FString& PlatformName)
{
	const FString PlatformIniPath = SauceLabs::GetProjectPlatformEngineIniPath(PlatformName);
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PlatformIniPath))
	{
		// Check if directory is writable
		const FString Dir = FPaths::GetPath(PlatformIniPath);
		return !FPlatformFileManager::Get().GetPlatformFile().IsReadOnly(*Dir);
	}
	return !FPlatformFileManager::Get().GetPlatformFile().IsReadOnly(*PlatformIniPath);
}

bool SSauceReportChecklist::HasPlatformDataRouterUrl(const FString& PlatformName)
{
	const FString PlatformIniPath = SauceLabs::GetProjectPlatformEngineIniPath(PlatformName);
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PlatformIniPath))
	{
		return false;
	}

	FString Value;
	FConfigFile ConfigFile;
	ConfigFile.Read(PlatformIniPath);
	if (const FConfigSection* Section = ConfigFile.FindSection(TEXT("CrashReportClient")))
	{
		if (const FConfigValue* ConfigValue = Section->Find(TEXT("DataRouterUrl")))
		{
			Value = ConfigValue->GetValue();
		}
	}

	return !Value.IsEmpty();
}

FString SSauceReportChecklist::GetPlatformDataRouterUrl(const FString& PlatformName)
{
	const FString PlatformIniPath = SauceLabs::GetProjectPlatformEngineIniPath(PlatformName);
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*PlatformIniPath))
	{
		return FString();
	}

	FString Value;
	FConfigFile ConfigFile;
	ConfigFile.Read(PlatformIniPath);
	if (const FConfigSection* Section = ConfigFile.FindSection(TEXT("CrashReportClient")))
	{
		if (const FConfigValue* ConfigValue = Section->Find(TEXT("DataRouterUrl")))
		{
			Value = ConfigValue->GetValue();
		}
	}

	return Value;
}

bool SSauceReportChecklist::IsIncludeCrashReporterEnabled()
{
	bool bValue = false;
	FConfigFile ConfigFile;
	ConfigFile.Read(FPaths::ProjectConfigDir() / TEXT("DefaultGame.ini"));
	if (const FConfigSection* Section = ConfigFile.FindSection(TEXT("/Script/UnrealEd.ProjectPackagingSettings")))
	{
		if (const FConfigValue* ConfigValue = Section->Find(TEXT("IncludeCrashReporter")))
		{
			bValue = ConfigValue->GetValue().StartsWith(TEXT("true"), ESearchCase::IgnoreCase);
		}
	}

	return bValue;
}

bool SSauceReportChecklist::IsDefaultGameIniWritable()
{
	const FString DefaultGameIni = FPaths::ProjectConfigDir() / TEXT("DefaultGame.ini");
	return !FPlatformFileManager::Get().GetPlatformFile().IsReadOnly(*DefaultGameIni);
}

FReply SSauceReportChecklist::OnFixCrashReporterClicked()
{
	const FString DefaultGameIni = FPaths::ProjectConfigDir() / TEXT("DefaultGame.ini");
	if (!IsDefaultGameIniWritable())
	{
		UE_LOG(LogSauceReport, Error, TEXT("Cannot fix Crash Reporter setting: DefaultGame.ini is read-only"));
		return FReply::Handled();
	}

	// Update using the file directly 
	if (!SauceLabs::UpdateSectionKeyValueOnIniFile(DefaultGameIni, TEXT("/Script/UnrealEd.ProjectPackagingSettings"), 
		TEXT("IncludeCrashReporter"), true))
	{
		UE_LOG(LogSauceReport, Error, TEXT("Could not write the DefaultGame.ini file"));
		return FReply::Handled();
	}

	OnRefreshClicked();

	return FReply::Handled();
}

FReply SSauceReportChecklist::OnOpenSettingsClicked()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->ShowViewer("Project", "Plugins", "SauceReportSettings");
	}
	return FReply::Handled();
}

FReply SSauceReportChecklist::OnRefreshClicked()
{
	// TODO: should we also reload all the Platform/PlatformEngine.ini ?

	// Reload the packaging settings CDO to ensure we are in sync
	if (UProjectPackagingSettings* PackagingSettings = GetMutableDefault<UProjectPackagingSettings>())
	{
		FProperty* Property = UProjectPackagingSettings::StaticClass()->FindPropertyByName(
			GET_MEMBER_NAME_CHECKED(UProjectPackagingSettings, IncludeCrashReporter));
		PackagingSettings->ReloadConfig(PackagingSettings->StaticClass(), *PackagingSettings->GetDefaultConfigFilename(),
			UE::LCPF_None, Property);
		PackagingSettings->IncludeCrashReporter = IsIncludeCrashReporterEnabled();
		FConfigContext Context = FConfigContext::ForceReloadIntoGConfig();
		Context.bWriteDestIni = false;
		Context.Load(*UProjectPackagingSettings::StaticClass()->ClassConfigName.ToString());
		// Notify the system that a property has changed
		// This triggers the UI updates in the Project Settings window if it is open.
		FPropertyChangedEvent PropertyEvent(Property);
		PackagingSettings->PostEditChangeProperty(PropertyEvent);
	}

	RefreshChecklist();
	return FReply::Handled();
}

TSharedRef<SWidget> SSauceReportChecklist::BuildHeaderSection()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0, 0, 12, 0)
			[
				SNew(SBox)
				.WidthOverride(40)
				.HeightOverride(40)
				[
					SNew(SImage)
					.Image(FSauceReportStyle::Get().GetBrush("SauceReport.PluginAction"))
				]
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("HeaderTitle", "Error Reporting"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 18))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 4, 0, 0)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("HeaderSubtitle", "Configure Error Reporting with SauceLabs"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 10))
					.ColorAndOpacity(FSlateColor::UseSubduedForeground())
				]
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 12, 0, 0)
		[
			SNew(SSeparator)
			.Orientation(Orient_Horizontal)
		];
}

TSharedRef<SWidget> SSauceReportChecklist::BuildWarningsSection()
{
	TSharedRef<SVerticalBox> WarningsBox = SNew(SVerticalBox);

	bool bHasWarnings = false;

	if (!IsDefaultEngineIniWritable())
	{
		bHasWarnings = true;
		WarningsBox->AddSlot()
		.AutoHeight()
		.Padding(0, 4)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
			.BorderBackgroundColor(FLinearColor(0.8f, 0.4f, 0.0f, 0.3f))
			.Padding(FMargin(12, 8))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 8, 0)
				[
					SNew(SImage)
					.Image(FAppStyle::GetBrush("Icons.WarningWithColor"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("DefaultEngineReadOnlyWarning", "Warning: DefaultEngine.ini is read-only. Settings changes will not be saved."))
					.ColorAndOpacity(FLinearColor(1.0f, 0.7f, 0.0f))
					.AutoWrapText(true)
				]
			]
		];
	}

	if (!bHasWarnings)
	{
		return SNew(SSpacer);
	}

	return WarningsBox;
}

TSharedRef<SWidget> SSauceReportChecklist::BuildStatusIcon(bool bSuccess, bool bWarning) const
{
	FName BrushName;
	if (bSuccess)
	{
		BrushName = "Icons.SuccessWithColor";
	}
	else if (bWarning)
	{
		BrushName = "Icons.WarningWithColor";
	}
	else
	{
		BrushName = "Icons.ErrorWithColor";
	}

	return SNew(SBox)
		.WidthOverride(20)
		.HeightOverride(20)
		[
			SNew(SImage)
			.Image(FAppStyle::GetBrush(BrushName))
		];
}

TSharedRef<SWidget> SSauceReportChecklist::BuildDefaultEngineSection()
{
	const bool bHasUrl = HasDefaultDataRouterUrl();
	const bool bIsWritable = IsDefaultEngineIniWritable();
	const FString Url = GetDefaultDataRouterUrl();

	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(FMargin(12))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 8)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("DefaultEngineTitle", "Default Configuration"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 8, 0)
				[
					BuildStatusIcon(bHasUrl)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("DefaultEngineUrlLabel", "DataRouterUrl in DefaultEngine.ini"))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 2, 0, 0)
					[
						SNew(STextBlock)
						.Text(bHasUrl ? FText::FromString(Url) : LOCTEXT("NotConfigured", "Not configured"))
						.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
						.ColorAndOpacity(bHasUrl ? FSlateColor::UseForeground() : FSlateColor::UseSubduedForeground())
					]
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 8, 0)
				[
					BuildStatusIcon(bIsWritable, !bIsWritable)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(bIsWritable ? LOCTEXT("FileWritable", "DefaultEngine.ini is writable") : LOCTEXT("FileReadOnly", "DefaultEngine.ini is read-only"))
				]
			]
		];
}

TSharedRef<SWidget> SSauceReportChecklist::BuildPlatformRow(const FString& PlatformName) const
{
	const bool bHasUrl = HasPlatformDataRouterUrl(PlatformName);
	const bool bIsWritable = IsPlatformIniWritable(PlatformName);
	const FString Url = GetPlatformDataRouterUrl(PlatformName);
	const FString IniPath = SauceLabs::GetProjectPlatformEngineIniPath(PlatformName);
	const bool bFileExists = FPlatformFileManager::Get().GetPlatformFile().FileExists(*IniPath);

	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.DarkGroupBorder"))
		.Padding(FMargin(8))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 8, 0)
				[
					BuildStatusIcon(bHasUrl)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(FText::FromString(PlatformName))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 10))
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(bIsWritable ? LOCTEXT("Writable", "Writable") : LOCTEXT("ReadOnly", "Read-only"))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 9))
					.ColorAndOpacity(bIsWritable ? FLinearColor::Green : FLinearColor(1.0f, 0.5f, 0.0f))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(28, 4, 0, 0)
			[
				SNew(STextBlock)
				.Text(bHasUrl ? FText::FromString(Url) : (bFileExists ? LOCTEXT("NoUrlConfigured", "No URL configured") : LOCTEXT("IniNotExists", "INI file does not exist")))
				.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
				.ColorAndOpacity(bHasUrl ? FSlateColor::UseForeground() : FSlateColor::UseSubduedForeground())
			]
		];
}

TSharedRef<SWidget> SSauceReportChecklist::BuildPlatformsSection()
{
	TSharedRef<SVerticalBox> PlatformsBox = SNew(SVerticalBox);

	for (const FString& Platform : AvailablePlatforms)
	{
		PlatformsBox->AddSlot()
		.AutoHeight()
		.Padding(0, 2)
		[
			BuildPlatformRow(Platform)
		];
	}

	return SNew(SExpandableArea)
		.InitiallyCollapsed(true)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f))
		.Padding(FMargin(12))
		.HeaderContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::Format(LOCTEXT("PlatformsTitle", "Platform Configurations ({0} platforms)"), FText::AsNumber(AvailablePlatforms.Num())))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
		]
		.BodyContent()
		[
			SNew(SBox)
			.Padding(FMargin(0, 8, 0, 0))
			[
				PlatformsBox
			]
		];
}

TSharedRef<SWidget> SSauceReportChecklist::BuildCrashReporterSection()
{
	const bool bEnabled = IsIncludeCrashReporterEnabled();

	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(FMargin(12))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 8)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CrashReporterTitle", "Packaging Settings"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 8, 0)
				[
					BuildStatusIcon(bEnabled)
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("IncludeCrashReporterLabel", "Include Crash Reporter"))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 2, 0, 0)
					[
						SNew(STextBlock)
						.Text(bEnabled ? LOCTEXT("Enabled", "Enabled") : LOCTEXT("Disabled", "Disabled: Crash reports will not be sent"))
						.Font(FCoreStyle::GetDefaultFontStyle("Italic", 9))
						.ColorAndOpacity(bEnabled ? FLinearColor::Green : FLinearColor::Red)
					]
				]
			]
		];
}

TSharedRef<SWidget> SSauceReportChecklist::BuildActionsSection()
{
	const bool bCrashReporterEnabled = IsIncludeCrashReporterEnabled();
	const bool bDefaultGameWritable = IsDefaultGameIniWritable();

	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(FMargin(12))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 8)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ActionsTitle", "Actions"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 8, 0)
				[
					SNew(SButton)
					.Text(LOCTEXT("OpenSettingsButton", "Open Sauce Report Settings"))
					.OnClicked(this, &SSauceReportChecklist::OnOpenSettingsClicked)
					.ButtonStyle(FAppStyle::Get(), "PrimaryButton")
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("RefreshButton", "Refresh"))
					.OnClicked(this, &SSauceReportChecklist::OnRefreshClicked)
				]
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(8, 0, 0, 0)
				[
					SNew(SButton)
					.Text(LOCTEXT("FixButton", "Enable Crash Reporter"))
					.IsEnabled(!bCrashReporterEnabled && bDefaultGameWritable)
					.Visibility(bCrashReporterEnabled ? EVisibility::Collapsed : EVisibility::Visible)
					.OnClicked(this, &SSauceReportChecklist::OnFixCrashReporterClicked)
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE

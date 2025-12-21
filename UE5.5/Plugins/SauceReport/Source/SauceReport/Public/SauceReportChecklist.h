// Copyright Sauce Labs, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>

#include "Components/VerticalBox.h"

class SSauceReportChecklist : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSauceReportChecklist) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	// Check functions
	static bool IsPlatformIniWritable(const FString& PlatformName);
	static bool HasPlatformDataRouterUrl(const FString& PlatformName);
	static bool IsIncludeCrashReporterEnabled();
	static bool IsDefaultGameIniWritable();
	static bool HasDefaultDataRouterUrl();
	static FString GetDefaultDataRouterUrl();
	static bool IsDefaultEngineIniWritable();
	static FString GetPlatformDataRouterUrl(const FString& PlatformName);
	
	// Refresh the checklist data
	void RefreshChecklist();

	// Button handlers
	FReply OnFixCrashReporterClicked();
	FReply OnRefreshClicked();
	FReply OnOpenSettingsClicked();

	// UI builders
	TSharedRef<SWidget> BuildHeaderSection();
	TSharedRef<SWidget> BuildDefaultEngineSection();
	TSharedRef<SWidget> BuildPlatformsSection();
	TSharedRef<SWidget> BuildCrashReporterSection();
	TSharedRef<SWidget> BuildActionsSection();
	TSharedRef<SWidget> BuildWarningsSection();

	TSharedRef<SWidget> BuildStatusIcon(bool bSuccess, bool bWarning = false) const;
	TSharedRef<SWidget> BuildPlatformRow(const FString& PlatformName) const;

	// Cached data
	TArray<FString> AvailablePlatforms;
	TSharedPtr<SVerticalBox> MainContainer;
	TSharedPtr<SVerticalBox> WarningsContainer;
};

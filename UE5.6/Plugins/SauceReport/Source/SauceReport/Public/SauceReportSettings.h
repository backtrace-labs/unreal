// Copyright Sauce Labs, Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Engine/DeveloperSettings.h>
#include <Templates/SubclassOf.h>

#include "SauceReportSettings.generated.h"

struct FDataDrivenPlatformInfo;

UCLASS(Config = Editor, DefaultConfig)
class SAUCEREPORT_API USauceReportSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	virtual FName GetContainerName() const override { return NAME_None; }

	UPROPERTY(Config, EditDefaultsOnly, Category="Custom Settings")
	FString DefaultDataRouterUrl{};

	UPROPERTY(Config, EditDefaultsOnly, Category="Custom Settings", meta = (ToolTip = "Platform name -> URL"))
	TMap<FString, FString> PlatformsToDataRouterUrl{};
};

namespace SauceLabs
{
	FString GetProjectPlatformEngineIniPath(const FString& PlatformName);
	bool EnsureEnginePlatformIniExists(const FString& PlatformName);

	// Helper function to safely read, modify, and save a specific INI file
	// bypassing the global GConfig cache which can cause pathing issues.
	void UpdateSpecificIniFile(const FString& FilePath, const FString& Section, const FString& Key, const FString& Value);

	FString PlatformNamesToString(const TMap<FName, FDataDrivenPlatformInfo>& PlatformInfos);
/* SauceLabs */ }

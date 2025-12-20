// Copyright Sauce Labs, Inc. All Rights Reserved.

#include "SauceReportSettings.h"

#include <GenericPlatform/GenericPlatformFile.h>
#include <HAL/PlatformFileManager.h>
#include <Misc/ConfigCacheIni.h>
#include <Misc/DataDrivenPlatformInfoRegistry.h>
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>

#include "SauceReport.h"

namespace SauceLabs
{

FString SauceLabs::GetProjectPlatformEngineIniPath(const FString& PlatformName)
{
	return FPaths::ProjectConfigDir() / PlatformName / (PlatformName + TEXT("Engine.ini"));
}

bool EnsureEnginePlatformIniExists(const FString& PlatformName)
{
	FString IniPath = GetProjectPlatformEngineIniPath(PlatformName);
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*IniPath))
	{
		FString Dir = FPaths::GetPath(IniPath);
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.DirectoryExists(*Dir))
		{
			PlatformFile.CreateDirectoryTree(*Dir);
		}
		FString Content(FString::Printf(TEXT(";%s specific Engine settings\n"), *PlatformName));
		if (!FFileHelper::SaveStringToFile(Content, *IniPath))
		{
			UE_LOG(LogSauceReport, Error, TEXT("Failed to create %sEngine.ini at %s"), *PlatformName, *IniPath);
			return false;
		}
	}
	return true;
}

void UpdateSpecificIniFile(const FString& FilePath, const FString& Section, const FString& Key, const FString& Value)
{
	FConfigFile ConfigFile;
	// Attempt to read the file from disk. 
	// Even if empty, we want to start with what exists physically.
	ConfigFile.Read(FilePath);

	// Set the string in the local object
	ConfigFile.SetString(*Section, *Key, *Value);

	// Write the changes back to disk immediately
	if (ConfigFile.Write(FilePath))
	{
		UE_LOG(LogSauceReport, Log, TEXT("Successfully wrote DataRouterUrl to %s"), *FilePath);
	}
	else
	{
		UE_LOG(LogSauceReport, Error, TEXT("Failed to write to %s. Check file permissions."), *FilePath);
	}
}

FString PlatformNamesToString(const TMap<FName, FDataDrivenPlatformInfo>& PlatformInfos)
{
	FString Names;
	bool bFirst = true;
	for (const TTuple<FName, FDataDrivenPlatformInfo>& PlatformInfo : PlatformInfos)
	{
		if (bFirst)
		{
			bFirst = false;
		}
		else
		{
			Names += TEXT(", ");
		}

		Names += PlatformInfo.Key.ToString();
	}

	return Names;
}

/* SauceLabs */ }

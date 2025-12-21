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

bool UpdateSectionKeyValueOnIniFile(const FString& FilePath, const FString& Section, const FString& Key, const FString& Value)
{
	FConfigFile ConfigFile;
	ConfigFile.Read(FilePath);

	// Check current value to avoid unnecessary writes
	FString CurrentValue;
	bool bKeyExists = false;
	if (const FConfigSection* ConfigSection = ConfigFile.FindSection(*Section))
	{
		if (const FConfigValue* ConfigValue = ConfigSection->Find(*Key))
		{
			CurrentValue = ConfigValue->GetValue();
			bKeyExists = true;
		}
	}

	if (Value.IsEmpty())
	{
		// If key doesn't exist, nothing to remove
		if (!bKeyExists)
		{
			return true;
		}

		// Remove the key if value is empty so engine uses its default
		ConfigFile.RemoveKeyFromSection(*Section, *Key);

		// Remove the section if it's now empty
		if (const FConfigSection* ConfigSection = ConfigFile.FindSection(*Section))
		{
			if (ConfigSection->Num() == 0)
			{
				ConfigFile.Remove(*Section);
			}
		}
	}
	else
	{
		// If value is unchanged, skip writing
		if (bKeyExists && CurrentValue == Value)
		{
			return true;
		}

		ConfigFile.SetString(*Section, *Key, *Value);
	}

	if (ConfigFile.Write(FilePath))
	{
		UE_LOG(LogSauceReport, Verbose, TEXT("Successfully wrote string %s to %s"), *Key, *FilePath);
		return true;
	}

	UE_LOG(LogSauceReport, Error, TEXT("Failed to write string %s to %s. Check file permissions."), *Key, *FilePath);
	return false;
}

bool UpdateSectionKeyValueOnIniFile(const FString& FilePath, const FString& Section, const FString& Key, const bool Value)
{
	FConfigFile ConfigFile;
	ConfigFile.Read(FilePath);
	ConfigFile.SetBool(*Section, *Key, Value);
	if (ConfigFile.Write(FilePath))
	{
		UE_LOG(LogSauceReport, Verbose, TEXT("Successfully wrote bool %s to %s"), *Key, *FilePath);
		return true;
	}

	UE_LOG(LogSauceReport, Error, TEXT("Failed to write bool to %s. Check file permissions."), *FilePath);
	return false;
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

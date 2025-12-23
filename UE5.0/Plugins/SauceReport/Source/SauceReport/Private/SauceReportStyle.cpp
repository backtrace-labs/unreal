// Copyright Sauce Labs, Inc. All Rights Reserved.

#include "SauceReportStyle.h"

#include <Framework/Application/SlateApplication.h>
#include <Interfaces/IPluginManager.h>
#include <Slate/SlateGameResources.h>
#include <Styling/SlateStyleMacros.h>
#include <Styling/SlateStyleRegistry.h>

#include "SauceReport.h"


#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FSauceReportStyle::StyleInstance = nullptr;

void FSauceReportStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FSauceReportStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FSauceReportStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("SauceReportStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FSauceReportStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("SauceReportStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("SauceReport")->GetBaseDir() / TEXT("Resources"));

	Style->Set("SauceReport.PluginAction", new IMAGE_BRUSH_SVG(TEXT("SauceButtonIcon"), Icon20x20));
	return Style;
}

void FSauceReportStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FSauceReportStyle::Get()
{
	return *StyleInstance;
}

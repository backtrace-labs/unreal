// Sauce Labs Inc. All Rights Reserved.

#include "SauceSampleGameModeBase.h"

#include <Engine/GameViewportClient.h>
#include <Engine/LocalPlayer.h>
#include <Engine/World.h>
#include <Widgets/SOverlay.h>
#include <Widgets/SWeakWidget.h>
#include <Widgets/Layout/SBox.h>

#include "SauceReportCrashTest.h"


void ASauceSampleGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	const UWorld* const World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	if (APlayerController* const PC = IsValid(World) ? World->GetFirstPlayerController() : nullptr; IsValid(PC))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer(); IsValid(LocalPlayer))
		{
			if (UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient; IsValid(ViewportClient))
			{
				SAssignNew(CrashTestWidget, SOverlay)
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(666)
					.HeightOverride(666)
					[
						SNew(SSauceReportCrashTest)
					]
				];

				// Add the widget to the specific player's viewport
				ViewportClient->AddViewportWidgetContent(CrashTestWidget.ToSharedRef());

				// Show the mouse cursor so you can actually click the buttons
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}

void ASauceSampleGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CrashTestWidget.IsValid())
	{
		const UWorld* const World = GetWorld();
		if (const APlayerController* PC = IsValid(World) ? World->GetFirstPlayerController() : nullptr; IsValid(PC))
		{
			if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer(); IsValid(LocalPlayer))
			{
				if (UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient; IsValid(ViewportClient))
				{
					ViewportClient->RemoveViewportWidgetContent(CrashTestWidget.ToSharedRef());
				}
			}
		}

		CrashTestWidget.Reset();
	}


	Super::EndPlay(EndPlayReason);
}

// Sauce Labs Inc. All Rights Reserved.

#include "SauceReportCrashTest.h"

#include <EditorStyleSet.h>
#include <Widgets/Input/SButton.h>
#include <Widgets/Layout/SBorder.h>
#include <Widgets/Layout/SScrollBox.h>
#include <Widgets/Text/STextBlock.h>

#define LOCTEXT_NAMESPACE "SauceReportCrashTest"

void SSauceReportCrashTest::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(FMargin(23.0f))
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 18)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Title", "Crash Test Tools"))
					.Font(FCoreStyle::GetDefaultFontStyle("Bold", 32))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 0, 0, 8)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("Warning", "WARNING: These buttons will crash the editor! Use only for testing crash reporting."))
					.Font(FCoreStyle::GetDefaultFontStyle("Regular", 23))
					.ColorAndOpacity(FLinearColor::Red)
					.AutoWrapText(true)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 8)
				[
					SNew(SButton)
					.OnClicked(this, &SSauceReportCrashTest::OnCrashNullPointerClicked)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("NullPointer", "Null Pointer Dereference"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 23))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 4)
				[
					SNew(SButton)
					.OnClicked(this, &SSauceReportCrashTest::OnCrashCheckClicked)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Check", "check() Assertion"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 23))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 4)
				[
					SNew(SButton)
					.OnClicked(this, &SSauceReportCrashTest::OnCrashEnsureClicked)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("Ensure", "ensure() (Non-Fatal)"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 23))
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 4)
				[
					SNew(SButton)
					.OnClicked(this, &SSauceReportCrashTest::OnCrashFatalErrorClicked)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("FatalError", "UE_LOG Fatal Error"))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 23))
					]
				]
			]
		]
	];
}

FReply SSauceReportCrashTest::OnCrashNullPointerClicked()
{
	int32* NullPtr = nullptr;
	*NullPtr = 42; // Crash!
	return FReply::Handled();
}

FReply SSauceReportCrashTest::OnCrashCheckClicked()
{
	check(false); // Crash!
	return FReply::Handled();
}

FReply SSauceReportCrashTest::OnCrashEnsureClicked()
{
	ensureMsgf(false, TEXT("Test ensure triggered from Sauce Report Crash Test widget"));
	return FReply::Handled();
}

FReply SSauceReportCrashTest::OnCrashFatalErrorClicked()
{
	LowLevelFatalError(TEXT("Test fatal error from Sauce Report Crash Test widget"));
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

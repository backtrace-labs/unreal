// Sauce Labs Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>

class SSauceReportCrashTest : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSauceReportCrashTest) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FReply OnCrashNullPointerClicked();
	FReply OnCrashCheckClicked();
	FReply OnCrashEnsureClicked();
	FReply OnCrashFatalErrorClicked();
};

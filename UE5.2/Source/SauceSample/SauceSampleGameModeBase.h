// Sauce Labs Inc. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/GameModeBase.h>

#include "SauceSampleGameModeBase.generated.h"

class SWidget;

UCLASS()
class SAUCESAMPLE_API ASauceSampleGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	TSharedPtr<SWidget> CrashTestWidget{nullptr};
};

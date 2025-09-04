#pragma once

#include "CoreMinimal.h"
#include "GZGameplayEffect.h"
#include "Game/GZGameplayTags.h"
#include "GZGameEffectCooldown.generated.h"

UCLASS()
class PROJECTGZ_API UGZGameEffectCooldown : public UGZGameplayEffect
{
	GENERATED_BODY()

public:
	UGZGameEffectCooldown()
	{
		DurationPolicy = EGameplayEffectDurationType::HasDuration;
		DurationMagnitude = FScalableFloat(0.42f);
		Modifiers.Empty();
	}

protected:
};

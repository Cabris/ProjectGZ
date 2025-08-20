#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GZGameplayEffect.generated.h"

class UGZAbilitySystemComponent;

UCLASS()
class PROJECTGZ_API UGZGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	virtual void OnPropertiesUpdated();
};

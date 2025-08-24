#pragma once

#include "CoreMinimal.h"
#include "GZGameplayEffect.h"
#include "GZDamageEffect.generated.h"

UCLASS()
class PROJECTGZ_API UGZDamageEffect : public UGZGameplayEffect
{
	GENERATED_BODY()
	UGZDamageEffect();

public:
	FGameplayTag GetAppliedDamagePropertyTag() const
	{
		return AppliedDamagePropertyTag;
	}

protected:
	virtual void OnPropertiesUpdated() override;
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	FGameplayTag AppliedDamagePropertyTag;

private:
};

#pragma once

#include "CoreMinimal.h"
#include "GZGameplayEffect.h"
#include "Data/GZAbilityCostDataAsset.h"
#include "GZGameEffectCost.generated.h"

UCLASS()
class PROJECTGZ_API UGZGameEffectCost : public UGZGameplayEffect
{
	GENERATED_BODY()

public:
	UGZGameEffectCost();
	void RebuildModifiers();
protected:
	virtual void OnPropertiesUpdated() override;
	UPROPERTY(EditDefaultsOnly, Category="Cost")
	TObjectPtr<UGZAbilityCostDataAsset> CostDataAsset;
};

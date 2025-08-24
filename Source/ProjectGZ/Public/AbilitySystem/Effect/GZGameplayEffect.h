#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GZGameplayEffect.generated.h"

class UGZAttributeTagMap;
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
	FGameplayAttribute GetEffectAttributeByTag() const;

	UPROPERTY(EditDefaultsOnly, Category="Effect")
	FGameplayTag EffectedAttributePropertyTag;
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<UGZAttributeTagMap> AttributeTagMap;
};

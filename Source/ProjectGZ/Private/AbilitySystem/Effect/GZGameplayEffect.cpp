#include "AbilitySystem/Effect/GZGameplayEffect.h"

#include "AbilitySystem/GZAttributeSet.h"
#include "Data/GZAttributeTagMap.h"

void UGZGameplayEffect::PostLoad()
{
	Super::PostLoad();
	OnPropertiesUpdated();
}
#if WITH_EDITOR
void UGZGameplayEffect::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	OnPropertiesUpdated();
}
#endif

void UGZGameplayEffect::OnPropertiesUpdated()
{
}

FGameplayAttribute UGZGameplayEffect::GetEffectAttributeByTag() const
{
	FGameplayAttribute EffectAttribute = UGZAttributeSet::GetHealthAttribute();
	if (AttributeTagMap && AttributeTagMap->HasTag(EffectedAttributePropertyTag))
	{
		EffectAttribute = AttributeTagMap->GetAttributeByTag(EffectedAttributePropertyTag);
	}
	return EffectAttribute;
}

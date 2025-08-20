#include "AbilitySystem/Effect/GZGameplayEffect.h"

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

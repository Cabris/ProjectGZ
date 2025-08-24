#include "AbilitySystem/Effect/GZDamageEffect.h"
#include "AbilitySystem/GZAttributeSet.h"
#include "Game/GZGameplayTags.h"

UGZDamageEffect::UGZDamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;
	AppliedDamagePropertyTag = GZGameplayTags::Effect_DamageMagnitude;
}

void UGZDamageEffect::OnPropertiesUpdated()
{
	Super::OnPropertiesUpdated();
	Modifiers.Reset();
	FGameplayModifierInfo Info;
	Info.ModifierOp = EGameplayModOp::Additive;
	Info.Attribute = GetEffectAttributeByTag();

	FSetByCallerFloat SetByCallerFloatStruct;
	SetByCallerFloatStruct.DataName = NAME_None;
	SetByCallerFloatStruct.DataTag = GetAppliedDamagePropertyTag(); // 用 GameplayTag 作為 Key
	FGameplayEffectModifierMagnitude Mag(SetByCallerFloatStruct);
	Info.ModifierMagnitude = Mag;
	Modifiers.Add(Info);
}

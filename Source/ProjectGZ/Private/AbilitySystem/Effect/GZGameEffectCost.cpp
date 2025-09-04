#include "AbilitySystem/Effect/GZGameEffectCost.h"

UGZGameEffectCost::UGZGameEffectCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant; // 成本通常是即時生效
	Modifiers.Empty();
}

void UGZGameEffectCost::OnPropertiesUpdated()
{
	Super::OnPropertiesUpdated();
	RebuildModifiers();
}

void UGZGameEffectCost::RebuildModifiers()
{
	if (!IsValid(CostDataAsset))return;
	UGZAbilityCostDataAsset* InCostDataAsset = CostDataAsset.Get();
	
	Modifiers.Empty();

	for (const FSetByCallerCostRow& Row : InCostDataAsset->GetAbilityCosts())
	{
		if (!Row.bEnabled) continue;
		if (!Row.Attribute.IsValid()) continue;
		if (!Row.SetByCallerKey.IsValid()) continue;

		FGameplayModifierInfo Info;
		Info.Attribute = Row.Attribute;
		Info.ModifierOp = Row.Op;

		// 建立一個「SetByCaller」型態的 Magnitude
		FSetByCallerFloat SetByCallerFloatStruct;
		SetByCallerFloatStruct.DataName = NAME_None;
		SetByCallerFloatStruct.DataTag = Row.SetByCallerKey; // 用 GameplayTag 作為 Key
		FGameplayEffectModifierMagnitude Mag(SetByCallerFloatStruct);
		Info.ModifierMagnitude = Mag;

		Modifiers.Add(Info);
	}
}

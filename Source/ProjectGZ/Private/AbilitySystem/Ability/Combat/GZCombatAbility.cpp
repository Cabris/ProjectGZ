#include "AbilitySystem/Ability/Combat/GZCombatAbility.h"
#include "Equipment/GZEquipmentInstance.h"

UGZCombatAbility::UGZCombatAbility()
{
	ActivationPolicy= EAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy= EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UGZInventoryItemInstance* UGZCombatAbility::GetItemInstance() const
{
	auto EquipmentInstance = GetEquipmentInstance();
	if (!IsValid(EquipmentInstance))return nullptr;
	return EquipmentInstance->GetItemInstance();
}

UGZEquipmentInstance* UGZCombatAbility::GetEquipmentInstance() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	if (!Spec) return nullptr;
	auto SourceObject = Spec->SourceObject;
	if (SourceObject.IsValid())
	{
		return Cast<UGZEquipmentInstance>(SourceObject.Get());
	}
	return nullptr;
}
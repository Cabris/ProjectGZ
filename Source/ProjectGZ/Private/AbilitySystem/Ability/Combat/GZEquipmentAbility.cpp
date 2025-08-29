#include "AbilitySystem/Ability/Combat/GZEquipmentAbility.h"
#include "Equipment/GZEquipmentInstance.h"

UGZEquipmentAbility::UGZEquipmentAbility()
{
	ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UGZInventoryItemInstance* UGZEquipmentAbility::GetItemInstance() const
{
	auto EquipmentInstance = GetEquipmentInstance();
	if (!IsValid(EquipmentInstance))return nullptr;
	return EquipmentInstance->GetItemInstance();
}

UGZEquipmentInstance* UGZEquipmentAbility::GetEquipmentInstance() const
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

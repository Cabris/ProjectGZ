#include "AbilitySystem/Ability/Combat/GZWeaponAbility.h"

UGZWeaponInstance* UGZWeaponAbility::GetWeaponInstance() const
{
	auto Equipment = GetEquipmentInstance();
	if (IsValid(Equipment))
		return Cast<UGZWeaponInstance>(Equipment);
	return nullptr;
}

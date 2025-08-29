#include "AbilitySystem/Ability/Task/GZRangeWeaponTraceTask.h"

UGZRangeWeaponTraceTask* UGZRangeWeaponTraceTask::CreateRangeWeaponTrace(UGameplayAbility* OwningAbility)
{
	auto Task = NewAbilityTask<UGZRangeWeaponTraceTask>(OwningAbility, FName("RangeWeaponTrace"));
	return Task;
}

void UGZRangeWeaponTraceTask::Activate()
{
}

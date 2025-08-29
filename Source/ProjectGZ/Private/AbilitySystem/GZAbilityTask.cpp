#include "AbilitySystem/Ability/Task/GZAbilityTask.h"

UGZAbilityTask* UGZAbilityTask::CreateGZAbilityTaskForTest(UGameplayAbility* OwningAbility)
{
	UGZAbilityTask* Task = NewAbilityTask<UGZAbilityTask>(OwningAbility);
	return Task;
}

void UGZAbilityTask::Activate()
{
	ValidData.Broadcast(42);
}

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GZAbilityTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGZAbilityTaskDataSignature, const int32&, Data);

UCLASS()
class PROJECTGZ_API UGZAbilityTask : public UAbilityTask
{
	GENERATED_BODY()

public:
	//For BP test
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
		meta=(DisplayName = "GZAbilityTask(For Test)", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UGZAbilityTask* CreateGZAbilityTaskForTest(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	
	UPROPERTY(BlueprintAssignable)
	FGZAbilityTaskDataSignature ValidData;
};

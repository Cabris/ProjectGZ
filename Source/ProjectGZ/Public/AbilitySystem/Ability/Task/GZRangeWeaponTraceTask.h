#pragma once
#include "CoreMinimal.h"
#include "GZAbilityTask.h"
#include "Equipment/GZWeaponInstance.h"
#include "GZRangeWeaponTraceTask.generated.h"


UCLASS()
class PROJECTGZ_API UGZRangeWeaponTraceTask : public UGZAbilityTask
{
	GENERATED_BODY()
	public:
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
	meta=(DisplayName = "RangeWeaponTrace Task", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UGZRangeWeaponTraceTask* CreateRangeWeaponTrace(UGameplayAbility* OwningAbility);
	virtual void Activate() override;

private:
	FFireParams Params;
};

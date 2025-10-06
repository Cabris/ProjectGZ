#pragma once

#include "CoreMinimal.h"
#include "GZCustomCost.h"
#include "GZTagStackCost.generated.h"

UCLASS()
class PROJECTGZ_API UGZTagStackCost : public UGZCustomCost
{
	GENERATED_BODY()

public:
	UGZTagStackCost();

	virtual bool CheckCost_Internal(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                                const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) override;

	virtual void ApplyCost_Internal(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                                const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="StackCost")
	FScalableFloat Quantity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="StackCost")
	FGameplayTag StackTag;
};

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GZGameplayAbility.generated.h"

UENUM()
enum class EAbilityActivationPolicy:uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,
	// Continually try to activate the ability while the input is active.
	WhileInputActive,
	// Try to activate the ability when an avatar is assigned.
	OnSpawn,
	// Try to activate the ability when ability is given to ASC.
	OnGiven
};

UCLASS()
class PROJECTGZ_API UGZGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	EAbilityActivationPolicy ActivationPolicy;

private:
};

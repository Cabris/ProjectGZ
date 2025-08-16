#pragma once

#include "CoreMinimal.h"
#include "GZGameplayAbility.h"
#include "GZAttackAbility.generated.h"

UCLASS()
class PROJECTGZ_API UGZAttackAbility : public UGZGameplayAbility
{
	GENERATED_BODY()

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	void StartCharging();
	UFUNCTION()
	void OnChargeReleased(float TimeHeld);
	UFUNCTION()
	void OnMontageCompleted();
	void FireArrow(float ChargeTime);
	float ChargeStartTime;
};

#pragma once
#include "CoreMinimal.h"
#include "GZEquipmentAbility.h"
#include "GZBowAttackTestAbility.generated.h"

UCLASS()
class PROJECTGZ_API UGZBowAttackTestAbility : public UGZEquipmentAbility
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

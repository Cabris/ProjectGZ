#pragma once
#include "CoreMinimal.h"
#include "GZCombatAbility.h"
#include "Equipment/GZWeaponInstance.h"
#include "GZRangeAttackAbility.generated.h"

class UGZWeaponInstance;
//base class for ranged weapon fire ability: pistol, rifle, row.
UCLASS()
class PROJECTGZ_API UGZRangeAttackAbility : public UGZCombatAbility
{
	GENERATED_BODY()

public:
	UGZRangeAttackAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UGZWeaponInstance* GetWeaponInstance() const;
	UPROPERTY(EditDefaultsOnly, Category="Ability|Combat")
	FAttackFilter AttackFilter;

private:
	void StartFireTimer();
	void StopFireTimer();
	void FireBody();
	bool DoOneFire();
	
	FTimerHandle FireTimerHandle;
	UPROPERTY(Transient)
	TObjectPtr<UGZWeaponInstance> CachedWeaponInstance;
	FWeaponConfig CachedWeaponConfig;
};

#pragma once

#include "CoreMinimal.h"
#include "GZEquipmentAbility.h"
#include "Equipment/GZWeaponInstance.h"
#include "Equipment/WeaponConfig.h"
#include "GZWeaponAbility.generated.h"


class UGZAimMotionComponent;

UCLASS()
class PROJECTGZ_API UGZWeaponAbility : public UGZEquipmentAbility
{
	GENERATED_BODY()
public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
protected:
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	UPROPERTY(EditDefaultsOnly, Category="Ability|Combat")
	TSubclassOf<UGZDamageEffect> DamageEffectClass;
	UGZWeaponInstance* GetWeaponInstance() const;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Ability|Combat")
	TObjectPtr<UGZWeaponInstance> CachedWeaponInstance;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Ability|Combat")
	FWeaponConfig CachedWeaponConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability|Combat")
	FAttackFilter AttackFilter;
	UPROPERTY(EditDefaultsOnly, Category="Ability|Combat")
	FGameplayTagContainer CooldownTags;

private:
	void InitializeCachedData();
	UPROPERTY(Transient)
	TObjectPtr<UGZAimMotionComponent> AimMotionComponent = nullptr;
};

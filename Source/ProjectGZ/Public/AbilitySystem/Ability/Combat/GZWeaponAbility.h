#pragma once

#include "CoreMinimal.h"
#include "GZEquipmentAbility.h"
#include "Equipment/GZWeaponInstance.h"
#include "Equipment/WeaponConfig.h"
#include "GZWeaponAbility.generated.h"


UCLASS()
class PROJECTGZ_API UGZWeaponAbility : public UGZEquipmentAbility
{
	GENERATED_BODY()

protected:
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							   const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	UPROPERTY(EditDefaultsOnly, Category="Cooldown")
	FGameplayTagContainer CooldownTags;
	
	UPROPERTY(EditDefaultsOnly, Category="Ability|Combat")
	TSubclassOf<UGZDamageEffect> DamageEffectClass;
	UGZWeaponInstance* GetWeaponInstance() const;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Ability|Combat")
	TObjectPtr<UGZWeaponInstance> CachedWeaponInstance;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Ability|Combat")
	FWeaponConfig CachedWeaponConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability|Combat")
	FAttackFilter AttackFilter;
};

#include "AbilitySystem/Ability/Combat/GZWeaponAbility.h"

#include "AbilitySystem/Effect/GZGameEffectCooldown.h"

UGZWeaponInstance* UGZWeaponAbility::GetWeaponInstance() const
{
	auto Equipment = GetEquipmentInstance();
	if (IsValid(Equipment))
		return Cast<UGZWeaponInstance>(Equipment);
	return nullptr;
}

void UGZWeaponAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
										  const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (IsValid(CooldownGE) && CooldownGE->GetClass()->IsChildOf(UGZGameEffectCooldown::StaticClass()))
	{
		float CooldownTime = CachedWeaponConfig.FireInterval;
		FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel(Handle, ActorInfo));
		if (!Spec.IsValid()) return;
		Spec.Data->SetDuration(CooldownTime, true);
		// 與 GetCooldownTags() 回傳一致：把這些 Tag 動態授與出去
		for (const FGameplayTag& Tag : CooldownTags)
		{
			Spec.Data->DynamicGrantedTags.AddTag(Tag);
		}
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, Spec);
	}
	else
	{
		Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	}
}

const FGameplayTagContainer* UGZWeaponAbility::GetCooldownTags() const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (IsValid(CooldownGE) && CooldownGE->GetClass()->IsChildOf(UGZGameEffectCooldown::StaticClass()))
	{
		return CooldownTags.IsEmpty() ? nullptr : &CooldownTags;
	}
	else
	{
		return Super::GetCooldownTags();
	}
}

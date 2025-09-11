#include "AbilitySystem/Ability/Combat/GZWeaponAbility.h"
#include "AbilitySystem/Effect/GZGameEffectCooldown.h"
#include "Character/GZAimMotionComponent.h"
#include "ProjectGZ/ProjectGZ.h"

class UGZAimMotionComponent;

UGZWeaponInstance* UGZWeaponAbility::GetWeaponInstance() const
{
	auto Equipment = GetEquipmentInstance();
	if (IsValid(Equipment))
		return Cast<UGZWeaponInstance>(Equipment);
	return nullptr;
}

void UGZWeaponAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	InitializeCachedData();
}

void UGZWeaponAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	AimMotionComponent = ActorInfo->AvatarActor->GetComponentByClass<UGZAimMotionComponent>();
}

void UGZWeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	InitializeCachedData();
	//TODO: Send CameraPose Override Start event to Manager
	if (IsValid(AimMotionComponent) && IsLocalControlled())
	{
		AimMotionComponent->SetCameraRigTag(CachedWeaponConfig.CameraPoseOverrideTag, CachedWeaponConfig.CameraPoseRecoveryTime, true);
	}
}

void UGZWeaponAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	//TODO: Send CameraPose Override End event to Manager
	if (IsValid(AimMotionComponent) && IsLocalControlled())
	{
		AimMotionComponent->SetCameraRigTag(CachedWeaponConfig.CameraPoseOverrideTag, CachedWeaponConfig.CameraPoseRecoveryTime, false);
	}
}

void UGZWeaponAbility::InitializeCachedData()
{
	if (IsValid(CachedWeaponInstance))return;
	CachedWeaponInstance = GetWeaponInstance();
	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	UE_LOG(LogTemp, Warning, TEXT("UGZRangeAttackAbility::OnGiveAbility: CachedWeaponInstance= %p, "), CachedWeaponInstance.Get());
	if (IsValid(CachedWeaponInstance))
	{
		CachedWeaponConfig = CachedWeaponInstance->GetConfig();
	}
	else
	{
		CachedWeaponInstance = nullptr;
		DEBUG_PRINTF(TEXT("ActivateAbility Fail: WeaponInstance is Null"));
	}
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

#include "AbilitySystem/Ability/GZGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "Character/GZCharacterBase.h"


void UGZGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (ActivationPolicy == EAbilityActivationPolicy::OnGiven)
	{
		if (!Spec.IsActive() && ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UGZGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGZGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//UAbilityTask_WaitInputRelease::WaitInputRelease(this, bTestAlreadyReleased);
	//CommitAbility(Handle,ActorInfo,ActivationInfo);
}

void UGZGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

EAbilityActivationPolicy UGZGameplayAbility::GetActivationPolicy() const
{
	return ActivationPolicy;
}

UGZAbilitySystemComponent* UGZGameplayAbility::GetAbilitySystemComponent() const
{
	if (!CurrentActorInfo) return nullptr;
	auto ASC = CurrentActorInfo->AbilitySystemComponent.Get();
	return Cast<UGZAbilitySystemComponent>(ASC);
}

AGZCharacterBase* UGZGameplayAbility::GetCharacter() const
{
	if (!CurrentActorInfo) return nullptr;
	auto AvatarActor = CurrentActorInfo->AvatarActor.Get();
	if (!AvatarActor) return nullptr;
	return Cast<AGZCharacterBase>(AvatarActor);
}

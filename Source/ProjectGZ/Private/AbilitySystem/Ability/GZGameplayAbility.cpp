#include "AbilitySystem/Ability/GZGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/Cost/GZCustomCost.h"
#include "Character/GZCharacterBase.h"
#include "Interfactions/GZPawnFeatureInterface.h"


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
	const TCHAR* AvatarActorName = ActorInfo->AvatarActor.IsValid() ? *ActorInfo->AvatarActor->GetName() : TEXT("None");

	UE_LOG(LogTemp, Log, TEXT("[UGZGameplayAbility::OnGiveAbility] Ability:[%s], given to: %s, NetExecutionPolicy: %d"),
	       *GetName(), AvatarActorName, NetExecutionPolicy.GetIntValue());
}

void UGZGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
	const TCHAR* AvatarActorName = ActorInfo->AvatarActor.IsValid() ? *ActorInfo->AvatarActor->GetName() : TEXT("None");

	UE_LOG(LogTemp, Log, TEXT("[UGZGameplayAbility::OnRemoveAbility] Ability removed from: %s"), AvatarActorName);
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

bool UGZGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bParentCheck = Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
	if (!bParentCheck)
		return false;

	for (auto& CustomCost : CustomCosts)
	{
		bool check = CustomCost->CheckCost(this, Handle, ActorInfo, OptionalRelevantTags);
		if (!check)
			return false;
	}
	return true;
}

void UGZGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	for (auto& CustomCost : CustomCosts)
	{
		CustomCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
	}
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

UGZPawnFeatureComponent* UGZGameplayAbility::GetPawnFeature() const
{
	auto Char = GetCharacter();
	if (!Char) return nullptr;
	IGZPawnFeatureInterface* IPawnFeature = Cast<IGZPawnFeatureInterface>(Char);
	if (!IPawnFeature) return nullptr;
	return IPawnFeature->GetPawnFeature();
}

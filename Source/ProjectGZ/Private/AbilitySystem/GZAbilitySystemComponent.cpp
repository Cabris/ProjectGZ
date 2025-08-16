#include "AbilitySystem/GZAbilitySystemComponent.h"

#include "AbilitySystem/GZInputGameplayAbilitySet.h"
#include "AbilitySystem/Ability/GZGameplayAbility.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Game/GameplayEventMessage.h"
#include "Game/GZGameplayTags.h"

void UGZAbilitySystemComponent::OnAbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::OnEffectAppliedToSelf);
}

void UGZAbilitySystemComponent::AbilityInputPressed(FGameplayTag InputTag)
{
	for (FGameplayAbilitySpec Spec : ActivatableAbilities.Items)
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			UGZGameplayAbility* Ability = Cast<UGZGameplayAbility>(Spec.Ability);
			if (Ability->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
			{
				AbilitySpecInputPressed(Spec);
				//
			}
			else if (Ability->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
			{
				//
			}
		}
	}
}

void UGZAbilitySystemComponent::AbilityInputReleased(FGameplayTag InputTag)
{
	for (FGameplayAbilitySpec Spec : ActivatableAbilities.Items)
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(Spec);
			UGZGameplayAbility* Ability = Cast<UGZGameplayAbility>(Spec.Ability);
			if (Ability->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
			{
				//
			}
		}
	}
}

void UGZAbilitySystemComponent::PostProcessInput(float DeltaTime, bool bGamePaused)
{
}

void UGZAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);
	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		const UGameplayAbility* Ability = Spec.GetPrimaryInstance();
		const FPredictionKey PredictionKey = Ability == nullptr
			                                     ? Spec.ActivationInfo.GetActivationPredictionKey()
			                                     : Ability->GetCurrentActivationInfo().GetActivationPredictionKey();

		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, PredictionKey);
	}
}

void UGZAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);
	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputRelease ability task works.
	if (Spec.IsActive())
	{
		const UGameplayAbility* Ability = Spec.GetPrimaryInstance();
		const FPredictionKey PredictionKey = Ability == nullptr
			                                     ? Spec.ActivationInfo.GetActivationPredictionKey()
			                                     : Ability->GetCurrentActivationInfo().GetActivationPredictionKey();
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, PredictionKey);
	}
}

void UGZAbilitySystemComponent::ApplyInputAbilitySet(const UGZInputGameplayAbilitySet* InputAbilitySet, UObject* SourceObject,
                                                     TArray<FGameplayAbilitySpecHandle>& OutHandles)
{
	if (!InputAbilitySet)return;
	for (const FInputAbilityEntry& Entry : InputAbilitySet->GetInputAbilityList())
	{
		TSubclassOf<UGZGameplayAbility> AbilityClass = Entry.AbilityClass;
		int32 InLevel = Entry.AbilityLevel;
		FGameplayTag InputTag = Entry.InputTag;

		FGameplayAbilitySpec AbilitySpec(AbilityClass, InLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTag);
		FGameplayAbilitySpecHandle AbilitySpecHandle = GiveAbility(AbilitySpec);
		OutHandles.AddUnique(AbilitySpecHandle);
	}
}

void UGZAbilitySystemComponent::GiveEquipmentGrantedAbility(const FInputAbilityEntry& GrantedAbility,
                                                            UObject* SourceObject, TArray<FGameplayAbilitySpecHandle>& OutHandles)
{
	TSubclassOf<UGZGameplayAbility> AbilityClass = GrantedAbility.AbilityClass;
	int32 InLevel = GrantedAbility.AbilityLevel;
	FGameplayTag InputTag = GrantedAbility.InputTag;

	FGameplayAbilitySpec AbilitySpec(AbilityClass, InLevel);
	AbilitySpec.SourceObject = SourceObject;
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTag);
	FGameplayAbilitySpecHandle AbilitySpecHandle = GiveAbility(AbilitySpec);
	OutHandles.AddUnique(AbilitySpecHandle);
	/*TryActivateAbility(AbilitySpecHandle);
	AbilitySpecInputReleased(AbilitySpec);
	AbilitySpecInputPressed(AbilitySpec);
	HandleGameplayEvent(InputTag, nullptr);
	ClearAbility(AbilitySpecHandle);*/
}

void UGZAbilitySystemComponent::OnEffectAppliedToSelf(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec,
                                                      FActiveGameplayEffectHandle Handle)
{
	FGameplayTagContainer Tags;
	Spec.GetAllAssetTags(Tags);
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	FGZVerbMessage OutgoingMessage;
	OutgoingMessage.ContextTags = Tags;
	MessageSystem.BroadcastMessage(GZGameplayTags::MessageTag_Effect_Applied, OutgoingMessage);
}

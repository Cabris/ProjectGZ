#include "AbilitySystem/GZAbilitySystemComponent.h"

#include "AbilitySystem/GZInputGameplayAbilitySet.h"
#include "AbilitySystem/Ability/GZGameplayAbility.h"
#include "Equipment/GZEquipmentDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Game/GameplayEventMessage.h"
#include "Game/GZGameplayTags.h"
#include "ProjectGZ/ProjectGZ.h"


UGZAbilitySystemComponent::UGZAbilitySystemComponent()
{
	constexpr int32 ReserveNum = 10;
	PressedAbilitySpecHandles.Reserve(ReserveNum);
	ReleasedAbilitySpecHandles.Reserve(ReserveNum);
	HoldingAbilitySpecHandles.Reserve(ReserveNum);
}

void UGZAbilitySystemComponent::OnAbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.RemoveAll(this);
	//OnGameplayEffectAppliedDelegateToSelf is Called on server whenever a GE is applied to self. This includes instant and duration based GEs.
	//So we need to a client RPC to call on server and executed on the client
	//for a listening server, the client-implement will be called immediately
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientEffectApplied);
}

void UGZAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec,
	FActiveGameplayEffectHandle Handle)
{
	FGameplayTagContainer Tags;
	Spec.GetAllAssetTags(Tags);
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	FGZVerbMessage OutgoingMessage;
	OutgoingMessage.ContextTags = Tags;
	MessageSystem.BroadcastMessage(GZGameplayTags::MessageTag_Effect_Applied, OutgoingMessage);
}

void UGZAbilitySystemComponent::HandleAbilityInputPressed(FGameplayTag InputTag)
{
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			UGZGameplayAbility* Ability = Cast<UGZGameplayAbility>(Spec.Ability);
			if (!Ability)
			{
				Debug::Print(FString::Printf(TEXT("Cast to UGZGameplayAbility failed: %s, Tag: %s"),
				                             *Spec.Ability->GetName(), *InputTag.ToString()));
				continue;
			}
			PressedAbilitySpecHandles.AddUnique(Spec.Handle);
			HoldingAbilitySpecHandles.AddUnique(Spec.Handle);
		}
	}
}

void UGZAbilitySystemComponent::HandleAbilityInputReleased(FGameplayTag InputTag)
{
	HoldingAbilitySpecHandles.RemoveAll([InputTag,this](FGameplayAbilitySpecHandle SpecHandle)
	{
		auto Spec = FindAbilitySpecFromHandle(SpecHandle);
		if (!Spec)
			return false;
		return Spec->GetDynamicSpecSourceTags().HasTagExact(InputTag);
	});

	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			UGZGameplayAbility* Ability = Cast<UGZGameplayAbility>(Spec.Ability);
			if (!Ability)
			{
				Debug::Print(FString::Printf(TEXT("Cast to UGZGameplayAbility failed: %s, Tag: %s"),
				                             *Spec.Ability->GetName(), *InputTag.ToString()));
				continue;
			}
			ReleasedAbilitySpecHandles.AddUnique(Spec.Handle);
		}
	}
}

void UGZAbilitySystemComponent::HandlePostProcessInput(float DeltaTime, bool bGamePaused)
{
	//Initialized Cache
	//Cached Abilities to be actived

	HoldingAbilitySpecHandles.RemoveAll([this](const FGameplayAbilitySpecHandle& H)
	{
		return FindAbilitySpecFromHandle(H) == nullptr;
	});

	TArray<FGameplayAbilitySpecHandle> WillActiveAbilityHandles;
	WillActiveAbilityHandles.Reserve(10);
	for (FGameplayAbilitySpecHandle SpecHandle : PressedAbilitySpecHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle);
		if (!Spec)
			continue;
		UGZGameplayAbility* Ability = Cast<UGZGameplayAbility>(Spec->Ability);
		if (Ability->GetActivationPolicy() == EAbilityActivationPolicy::OnInputTriggered)
		{
			WillActiveAbilityHandles.Add(Spec->Handle);
		}
	}

	for (FGameplayAbilitySpecHandle SpecHandle : HoldingAbilitySpecHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle);
		if (!Spec)
			continue;
		UGZGameplayAbility* Ability = Cast<UGZGameplayAbility>(Spec->Ability);
		if (Ability->GetActivationPolicy() == EAbilityActivationPolicy::WhileInputActive)
		{
			WillActiveAbilityHandles.Add(Spec->Handle);
		}
	}

	for (FGameplayAbilitySpecHandle SpecHandle : WillActiveAbilityHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle);
		if (!Spec)
			continue;
		if (!Spec->IsActive())
		{
			constexpr bool bAllowRemoteActivation = true;
			bool bIsSuccesses = TryActivateAbility(SpecHandle, bAllowRemoteActivation);
			Debug::Print(FString::Printf(TEXT("TryActivateAbility: bIsSuccesses: %d, Ability: %s"), bIsSuccesses, *Spec->Ability->GetName()));
		}
	}

	for (FGameplayAbilitySpecHandle SpecHandle : PressedAbilitySpecHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle);
		if (!Spec)
			continue;
		AbilitySpecInputPressed(*Spec);
	}

	for (FGameplayAbilitySpecHandle SpecHandle : ReleasedAbilitySpecHandles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(SpecHandle);
		if (!Spec)
			continue;
		AbilitySpecInputReleased(*Spec);
	}

	//Clean up
	PressedAbilitySpecHandles.Reset();
	ReleasedAbilitySpecHandles.Reset();
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
	else
	{
		Debug::Print(FString::Printf(TEXT("AbilitySpecInputPressed: Ability: %s is not Active"), *Spec.Ability->GetName()));
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
	else
	{
		Debug::Print(FString::Printf(TEXT("AbilitySpecInputReleased: Ability: %s is not Active"), *Spec.Ability->GetName()));
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

	if (!IsValid(AbilityClass))
	{
		Debug::Print(TEXT("GiveEquipmentGrantedAbility: AbilityClass is not Valid"));
		return;
	}
	
	FGameplayAbilitySpec AbilitySpec(AbilityClass, InLevel);
	AbilitySpec.SourceObject = SourceObject;
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTag);
	FGameplayAbilitySpecHandle AbilitySpecHandle = GiveAbility(AbilitySpec);
	OutHandles.AddUnique(AbilitySpecHandle);

	auto CDO = AbilityClass.GetDefaultObject();
	if (CDO->GetActivationPolicy()== EAbilityActivationPolicy::OnGiven)
	{
		TryActivateAbility(AbilitySpecHandle);
	}
	
	/*TryActivateAbility(AbilitySpecHandle);
	AbilitySpecInputReleased(AbilitySpec);
	AbilitySpecInputPressed(AbilitySpec);
	HandleGameplayEvent(InputTag, nullptr);
	ClearAbility(AbilitySpecHandle);*/
}


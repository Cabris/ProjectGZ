#include "AbilitySystem/Ability/GZInteractAbility.h"
#include "Interfactions/GZInteractable.h"

UGZInteractAbility::UGZInteractAbility()
{
	if (InteractionTag.IsValid())
	{
		FAbilityTriggerData Trigger;
		Trigger.TriggerTag = InteractionTag; // 事件 Tag
		Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(Trigger);
	}
}

void UGZInteractAbility::PostLoad()
{
	Super::PostLoad();
	if (HasAnyFlags(RF_ClassDefaultObject))
		OnInteractionTagChanged();
}

#if WITH_EDITOR
void UGZInteractAbility::PostEditChangeProperty(struct FPropertyChangedEvent& E)
{
	Super::PostEditChangeProperty(E);
	if (E.Property && E.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UGZInteractAbility, InteractionTag))
	{
		OnInteractionTagChanged();
	}
}
#endif

void UGZInteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	bool isValid = true;
	if (!InteractionTag.IsValid())
	{
		isValid = false;
	}
	if (!TriggerEventData)
	{
		UE_LOG(LogTemp, Error, TEXT("TriggerEventData is NULL!"));
		isValid = false;
	}
	else if (!TriggerEventData->Target)
	{
		UE_LOG(LogTemp, Error, TEXT("TriggerEventData->Target is NULL!"));
		isValid = false;
	}
	else if (!TriggerEventData->Target->Implements<UGZInteractable>())
	{
		UE_LOG(LogTemp, Error, TEXT("TriggerEventData->Target do not implements UGZInteractable!"));
		isValid = false;
	}
	AActor* TargetActor = const_cast<AActor*>(TriggerEventData->Target.Get());
	IGZInteractable* Interactable = Cast<IGZInteractable>(TargetActor);
	//auto WorldPosition = Interactable->GetWorldPosition();
	bool IsSuccessfulActive = ActivateAbilityInternal(Handle, ActorInfo, ActivationInfo, TriggerEventData, TargetActor, Interactable);
	if (!IsSuccessfulActive)
	{
		UE_LOG(LogTemp, Error, TEXT("ActivateAbilityInternal return false!"));
		isValid = false;
	}
	if (!isValid)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		//EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

bool UGZInteractAbility::IsInteractableActor(AActor* Target) const
{
	return Target && Target->Implements<UGZInteractable>();
}

void UGZInteractAbility::OnInteractionTagChanged()
{
	if (InteractionTag.IsValid())
	{
		AbilityTriggers.Reset();
		FAbilityTriggerData Trigger;
		Trigger.TriggerTag = InteractionTag;
		Trigger.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(Trigger);
	}
}

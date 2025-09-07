#pragma once

#include "CoreMinimal.h"
#include "GZGameplayAbility.h"
#include "GZInteractAbility.generated.h"

class IGZInteractable;

UCLASS()
class PROJECTGZ_API UGZInteractAbility : public UGZGameplayAbility
{
	GENERATED_BODY()

public:

	UGZInteractAbility();
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& E) override;
#endif
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	FGameplayTag GetInteractionTag() const
	{
		return InteractionTag;
	}

protected:
	//Return False will auto CancelAbility
	virtual bool ActivateAbilityInternal(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                     const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData,
	                                     AActor* InteractableActor, IGZInteractable* Interactable)
	{
		return false;
	}

	bool IsInteractableActor(AActor* Target) const;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FGameplayTag InteractionTag;

private:
	void OnInteractionTagChanged();
};

#pragma once

#include "CoreMinimal.h"
#include "GZInteractAbility.h"
#include "UGZCollectItemAbility.generated.h"

class UGZInventoryManagerComponent;
//Ability to pickup a item, loot treature box...
UCLASS(BlueprintType)
class PROJECTGZ_API UUGZCollectItemAbility : public UGZInteractAbility
{
	GENERATED_BODY()

public:
	virtual bool ActivateAbilityInternal(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                     const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData,
	                                     AActor* InteractableActor, IGZInteractable* Interactable) override;

protected:
	UGZInventoryManagerComponent* GetInventoryManager() const;
};

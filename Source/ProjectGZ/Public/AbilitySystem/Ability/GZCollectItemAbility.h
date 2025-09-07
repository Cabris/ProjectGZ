#pragma once

#include "CoreMinimal.h"
#include "GZInteractAbility.h"
#include "GZCollectItemAbility.generated.h"

class IGZCollectable;
class UGZInventoryItemDefinition;
class UGZInventoryManagerComponent;
//Ability to pickup a item, loot treasure box...
UCLASS(BlueprintType)
class PROJECTGZ_API UGZCollectItemAbility : public UGZInteractAbility
{
	GENERATED_BODY()

public:
	virtual bool ActivateAbilityInternal(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                     const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData,
	                                     AActor* InteractableActor, IGZInteractable* Interactable) override;

protected:
	UGZInventoryManagerComponent* GetInventoryManager() const;
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCollectItemFX(const AActor* Actor);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayCollectItemFailFX();
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bTryEquipItem = true;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bConsumeItemQuantity = true;

private:
 	bool CollectItemTask();
	bool CollectItemToInventory(IGZCollectable* Collectable) const;
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
};

#pragma once

#include "CoreMinimal.h"
#include "GZInteractAbility.h"
#include "GZCollectItemAbility.generated.h"

class UGZInventoryManagerComponent;
//Ability to pickup a item, loot treature box...
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
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	bool bConsumeItemQuantity = true;
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TObjectPtr<UAnimMontage> CollectMontage;

private:
	bool CollectItem();
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageInterrupted();
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;
};

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Effect/GZGameplayEffect.h"
#include "GZGameplayAbility.generated.h"

class UGZPawnFeatureComponent;
class UGZAbilityCostDataAsset;
class UGZGameplayEffect;
class AGZCharacterBase;
class UGZAbilitySystemComponent;

UENUM()
enum class EAbilityActivationPolicy:uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,
	// Continually try to activate the ability while the input is active.
	WhileInputActive,
	// Try to activate the ability when ability is given to ASC.
	OnGiven
};

//base class for GameplayAbility
UCLASS(Blueprintable, BlueprintType)
class PROJECTGZ_API UGZGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	EAbilityActivationPolicy GetActivationPolicy() const
	{
		return ActivationPolicy;
	}

protected:
	UGZAbilitySystemComponent* GetAbilitySystemComponent() const;
	//get avatar actor 
	AGZCharacterBase* GetCharacter() const;
	//UGZPawnFeatureComponent knows AbilitySystemComponent, EquipmentManagerComponent, InventoryManagerComponent
	UGZPawnFeatureComponent* GetPawnFeature() const;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly, Category="Ability")
	EAbilityActivationPolicy ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;

};

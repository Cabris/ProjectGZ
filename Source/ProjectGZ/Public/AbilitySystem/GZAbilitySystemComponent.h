// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GZAbilitySystemComponent.generated.h"

struct FInputAbilityEntry;
class UGZInputGameplayAbilitySet;

UCLASS()
class PROJECTGZ_API UGZAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void ApplyInputAbilitySet(const UGZInputGameplayAbilitySet* InputAbilitySet, UObject* SourceObject, TArray<FGameplayAbilitySpecHandle>& OutHandles);
	void GiveEquipmentGrantedAbility(const FInputAbilityEntry& GrantedAbility, UObject* SourceObject,
	                                 TArray<FGameplayAbilitySpecHandle>& OutHandles);
	void OnEffectAppliedToSelf(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	virtual void OnAbilityActorInfoSet();
	void AbilityInputPressed(FGameplayTag InputTag);
	void AbilityInputReleased(FGameplayTag InputTag);
	void PostProcessInput(float DeltaTime, bool bGamePaused);
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:
	
};

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
	UGZAbilitySystemComponent();

	//Init Abilities that trigger by InputTag,SourceObject is GZPlayerState
	void ApplyInputAbilitySet(const UGZInputGameplayAbilitySet* InputAbilitySet, UObject* SourceObject, TArray<FGameplayAbilitySpecHandle>& OutHandles);
	//Grant Ability when given a Weapon or Equipment, SourceObject: Equipment instance
	void GiveEquipmentGrantedAbility(const FInputAbilityEntry& GrantedAbility, UObject* SourceObject,
	                                 TArray<FGameplayAbilitySpecHandle>& OutHandles);

	//Method called after AbilityActorInfo is Set
	virtual void OnAbilityActorInfoSet();

	//Call by AGZPlayerController::AbilityInputPressed Event
	void HandleAbilityInputPressed(FGameplayTag InputTag);
	//Call by AGZPlayerController::AbilityInputReleased Event
	void HandleAbilityInputReleased(FGameplayTag InputTag);
	//Call by AGZPlayerController::PostProcessInput ,Method called after processing input
	void HandlePostProcessInput(float DeltaTime, bool bGamePaused);

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

protected:
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);

	//Cache Spec for AbilitySpecInputPressed, will be consumed in HandlePostProcessInput
	TArray<FGameplayAbilitySpecHandle> PressedAbilitySpecHandles;
	//Cache Spec for HandleAbilityInputReleased, will be consumed in HandlePostProcessInput
	TArray<FGameplayAbilitySpecHandle> ReleasedAbilitySpecHandles;
	//Ele will be removed when HandleAbilityInputReleased
	TArray<FGameplayAbilitySpecHandle> HoldingAbilitySpecHandles;
};

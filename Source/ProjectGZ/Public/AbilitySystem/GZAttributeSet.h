// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GZAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class FLifetimeProperty;
struct FGameplayEffectModCallbackData;

UCLASS()
class PROJECTGZ_API UGZAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGZAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_MaxHealth, Category="Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UGZAttributeSet, MaxHealth)
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_Health, Category="Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGZAttributeSet, Health)
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	FGameplayAttribute GetAttributeByTag(FGameplayTag Tag) const;

protected:
	using StaticFunctionPtr = FGameplayAttribute(*)();
	TMap<FGameplayTag, StaticFunctionPtr> TagAttributeMap;

private:
};

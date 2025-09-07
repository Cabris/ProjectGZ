// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfactions/GZAbilitySystemInterface.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "GZPlayerState.generated.h"
class UGZWeaponSlotComponent;
class UGZEquipmentManagerComponent;
class UGZInventoryManagerComponent;
class UGZAbilitySystemComponent;
class UGZPawnFeatureComponent;

UCLASS()
class PROJECTGZ_API AGZPlayerState : public APlayerState, public IGZAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGZPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UGZAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UGZAttributeSet* GetAttributeSet() const override;
	void UpdateCurrentEquipmentTag(FGameplayTag NewEquipmentTag);
protected:
	UPROPERTY(VisibleAnywhere, Category = "PawnFeature|Ability")
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, Category = "PawnFeature|Ability")
	TObjectPtr<UGZAttributeSet> AttributeSet;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentEquipmentTag, Category = "Animations")
	FGameplayTag CurrentEquipmentTag;

private:
	UFUNCTION()
	void OnRep_CurrentEquipmentTag(const FGameplayTag& OldEquipmentTag);
};

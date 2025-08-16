// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interfactions/GZAbilitySystemInterface.h"
#include "Interfactions/GZPawnFeatureInterface.h"
#include "GZPlayerState.generated.h"
class UGZWeaponMenuComponent;
class UGZEquipmentManagerComponent;
class UGZInventoryManagerComponent;
class UGZAbilitySystemComponent;
class UGZPawnFeatureComponent;

UCLASS()
class PROJECTGZ_API AGZPlayerState : public APlayerState, public IGZAbilitySystemInterface, public IGZPawnFeatureInterface
{
	GENERATED_BODY()

public:
	AGZPlayerState();

	FORCEINLINE virtual UGZAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

	FORCEINLINE virtual UGZAttributeSet* GetAttributeSet() const override
	{
		return AttributeSet;
	}

	virtual UGZPawnFeatureComponent* GetPawnFeature() override
	{
		return PawnFeature;
	}

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "PawnFeature")
	TObjectPtr<UGZPawnFeatureComponent> PawnFeature;
	
	UPROPERTY(VisibleAnywhere, Category = "PawnFeature|Ability")
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, Category = "PawnFeature|Ability")
	TObjectPtr<UGZAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, Category = "PawnFeature|Manager")
	TObjectPtr<UGZInventoryManagerComponent> InventoryManager;
	UPROPERTY(VisibleAnywhere, Category = "PawnFeature|Manager")
	TObjectPtr<UGZEquipmentManagerComponent> EquipmentManager;
	UPROPERTY(VisibleAnywhere, Category = "PawnFeature|Manager")
	TObjectPtr<UGZWeaponMenuComponent> WeaponMenu;
};

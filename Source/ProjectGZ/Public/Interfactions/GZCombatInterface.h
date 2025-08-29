// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "GZCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UGZCombatInterface : public UInterface
{
	GENERATED_BODY()
};


class PROJECTGZ_API IGZCombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="CombatInterface")
	FName GetCombatSocketName();
	UFUNCTION(BlueprintNativeEvent, Category="CombatInterface")
	void OnEquipmentTagChanged(FGameplayTag EquipmentTag);
};

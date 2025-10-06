// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Cost/GZTagStackCost.h"

#include "AbilitySystem/Ability/Combat/GZEquipmentAbility.h"
#include "Game/GZGameplayTags.h"
#include "Inventory/GZInventoryItemInstance.h"

UGZTagStackCost::UGZTagStackCost()
{
	Quantity.SetValue(1.0f);
	//StackTag=GZGameplayTags::Item_Stack_Ammo;
	StackTag = FGameplayTag();
}

bool UGZTagStackCost::CheckCost_Internal(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags)
{
	const UGZEquipmentAbility* EquipmentAbility = Cast<UGZEquipmentAbility>(Ability);
	if (EquipmentAbility)
	{
		UGZInventoryItemInstance* ItemInstance = EquipmentAbility->GetItemInstance();
		if (ItemInstance && ItemInstance->ContainsStackTag(StackTag))
		{
			int32 NumStacks = FMath::TruncToInt(Quantity.GetValue());
			bool bCanApplyCost = ItemInstance->GetStackByTag(StackTag) >= NumStacks;
			return bCanApplyCost;
		}
	}
	return false;
}

void UGZTagStackCost::ApplyCost_Internal(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	const UGZEquipmentAbility* EquipmentAbility = Cast<UGZEquipmentAbility>(Ability);
	if (EquipmentAbility)
	{
		UGZInventoryItemInstance* ItemInstance = EquipmentAbility->GetItemInstance();
		if (ItemInstance && ItemInstance->ContainsStackTag(StackTag))
		{
			int32 NumStacks = FMath::TruncToInt(Quantity.GetValue());
			int32 CurrentStacks = ItemInstance->GetStackByTag(StackTag);
			ItemInstance->SetStackByTag(StackTag, CurrentStacks - NumStacks);
		}
	}
}


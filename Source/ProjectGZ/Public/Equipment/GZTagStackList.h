// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GZTagStackList.generated.h"

USTRUCT(BlueprintType)
struct FGZTagStackEntry : public FFastArraySerializerItem
{
	FGZTagStackEntry()
	{
	}

	FGZTagStackEntry(const FGameplayTag& Tag, int32 Count)
		: ItemTag(Tag),
		  StackCount(Count)
	{
	}

	GENERATED_BODY()
	FGameplayTag ItemTag;
	int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FGZTagStackList : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	void AddItemStack(FGameplayTag Tag, int32 StackCount);
	void RemoveItemStack(FGameplayTag Tag, int32 StackCount);

	void SetItemStackCount(FGameplayTag Tag, int32 StackCount);
	
	uint8 GetItemStackCount(FGameplayTag Tag) const
	{
		return ItemStack.FindRef(Tag);
	}

	bool ContainsItemStack(FGameplayTag Tag) const
	{
		return ItemStack.Contains(Tag);
	}

	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGZTagStackEntry, FGZTagStackList>(
			Items, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FGZTagStackEntry> Items;
	TMap<FGameplayTag, int32> ItemStack;
};

//this is required for NetDeltaSerialize can be call
template <>
struct TStructOpsTypeTraits<FGZTagStackList> : public TStructOpsTypeTraitsBase2<FGZTagStackList>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

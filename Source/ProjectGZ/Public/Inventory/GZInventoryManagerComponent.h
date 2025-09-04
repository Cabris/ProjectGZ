#pragma once

#include "CoreMinimal.h"
#include "GZInventoryItemInstance.h"
#include "Character/GZPawnComponent.h"
#include "Components/ActorComponent.h"
#include "GZInventoryManagerComponent.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnListReplicatedSingnature, const TArray<FGZInventoryEntry>&, const TArrayView<int32>&/*RemovedIndices*/,
                             int32/*FinalSize*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryListItemUpdatedSingnature, UGZInventoryItemInstance*, ItemInstance);

class UGZInventoryManagerComponent;

USTRUCT(BlueprintType)
struct FGZInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	const TObjectPtr<UGZInventoryItemInstance>& GetItemInstance() const
	{
		return ItemInstance;
	}

private:
	friend struct FGZInventoryList;
	friend UGZInventoryManagerComponent;
	UPROPERTY()
	TObjectPtr<UGZInventoryItemInstance> ItemInstance = nullptr;
};

USTRUCT(BlueprintType)
struct FGZInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()
	FGZInventoryList()
	{
		UE_LOG(LogTemp, Warning, TEXT("FGZInventoryList() : %p"), this);
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		UE_LOG(LogTemp, Error, TEXT("=== NetDeltaSerialize Entry ==="));
		UE_LOG(LogTemp, Error, TEXT("Items.Num(): %d"), Items.Num());
		UE_LOG(LogTemp, Error, TEXT("bSupportsFastArrayDeltaStructSerialization: %s"),
		       DeltaParms.bSupportsFastArrayDeltaStructSerialization ? TEXT("YES") : TEXT("NO"));

		// 檢查DeltaFlags狀態
		EFastArraySerializerDeltaFlags CurrentFlags = GetDeltaSerializationFlags();
		UE_LOG(LogTemp, Error, TEXT("DeltaFlags: %d"), (int32)CurrentFlags);
		UE_LOG(LogTemp, Error, TEXT("HasDeltaBeenRequested: %s"),
		       EnumHasAnyFlags(CurrentFlags, EFastArraySerializerDeltaFlags::HasDeltaBeenRequested) ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTemp, Error, TEXT("IsUsingDeltaSerialization: %s"),
		       EnumHasAnyFlags(CurrentFlags, EFastArraySerializerDeltaFlags::IsUsingDeltaSerialization) ? TEXT("YES") : TEXT("NO"));
		UE_LOG(LogTemp, Error, TEXT("HasBeenSerialized: %s"),
		       EnumHasAnyFlags(CurrentFlags, EFastArraySerializerDeltaFlags::HasBeenSerialized) ? TEXT("YES") : TEXT("NO"));

		if (DeltaParms.Writer)
		{
			UE_LOG(LogTemp, Error, TEXT("WRITING - Checking each item before serialization"));
			for (int32 i = 0; i < Items.Num(); i++)
			{
				const FGZInventoryEntry& Entry = Items[i];
				UE_LOG(LogTemp, Error, TEXT("  Item[%d]: RepID=%d, RepKey=%d, ItemInstance=%p"),
				       i, Entry.ReplicationID, Entry.ReplicationKey, Entry.ItemInstance.Get());

				if (IsValid(Entry.ItemInstance))
				{
					UGZInventoryItemInstance* Instance = Entry.ItemInstance.Get();
					UE_LOG(LogTemp, Error, TEXT("    ItemInstance details:"));
					UE_LOG(LogTemp, Error, TEXT("    - IsSupportedForNetworking: %s"),
					       Instance->IsSupportedForNetworking() ? TEXT("YES") : TEXT("NO"));
					UE_LOG(LogTemp, Error, TEXT("    - ItemDefinitionClass: %p"),
					       Instance->GetItemDefinitionClass().Get());
					UE_LOG(LogTemp, Error, TEXT("    - Outer: %p"), Instance->GetOuter());
					UE_LOG(LogTemp, Error, TEXT("    - Class: %s"), *Instance->GetClass()->GetName());
				}
			}
		}

		// 嘗試序列化
		bool result = FFastArraySerializer::FastArrayDeltaSerialize<FGZInventoryEntry, FGZInventoryList>(
			Items, DeltaParms, *this);

		UE_LOG(LogTemp, Error, TEXT("=== NetDeltaSerialize Result: %s ==="), result ? TEXT("SUCCESS") : TEXT("FAILED"));

		if (!result)
		{
			UE_LOG(LogTemp, Error, TEXT("SERIALIZATION FAILED - Additional Debug Info:"));
			UE_LOG(LogTemp, Error, TEXT("DeltaParms.Struct: %p"), DeltaParms.Struct);
			UE_LOG(LogTemp, Error, TEXT("DeltaParms.Data: %p"), DeltaParms.Data);
			UE_LOG(LogTemp, Error, TEXT("DeltaParms.NetSerializeCB: %p"), DeltaParms.NetSerializeCB);

			if (DeltaParms.Writer)
			{
				UE_LOG(LogTemp, Error, TEXT("Writer Error State: %s"), DeltaParms.Writer->IsError() ? TEXT("HAS_ERROR") : TEXT("NO_ERROR"));
				UE_LOG(LogTemp, Error, TEXT("Writer GetNumBits: %lld"), DeltaParms.Writer->GetNumBits());
			}

			if (DeltaParms.Reader)
			{
				UE_LOG(LogTemp, Error, TEXT("Reader Error State: %s"), DeltaParms.Reader->IsError() ? TEXT("HAS_ERROR") : TEXT("NO_ERROR"));
				UE_LOG(LogTemp, Error, TEXT("Reader Pos: %lld"), DeltaParms.Reader->GetPosBits());
			}

			// 檢查是否是特定類型的序列化問題
			UE_LOG(LogTemp, Error, TEXT("Checking for common serialization issues:"));

			// 檢查UObject序列化問題
			for (const FGZInventoryEntry& Entry : Items)
			{
				if (IsValid(Entry.ItemInstance))
				{
					UGZInventoryItemInstance* Instance = Entry.ItemInstance.Get();

					// 檢查Outer是否正確
					if (!IsValid(Instance->GetOuter()))
					{
						UE_LOG(LogTemp, Error, TEXT("FOUND ISSUE: ItemInstance has invalid Outer!"));
					}

					// 檢查Class是否有效
					if (!IsValid(Instance->GetClass()))
					{
						UE_LOG(LogTemp, Error, TEXT("FOUND ISSUE: ItemInstance has invalid Class!"));
					}

					// 檢查是否在正確的world中
					if (Instance->GetWorld() != (IsValid(OwnerComponent) ? OwnerComponent->GetWorld() : nullptr))
					{
						UE_LOG(LogTemp, Error, TEXT("POTENTIAL ISSUE: ItemInstance world mismatch"));
					}
				}
			}
		}

		return result;
	}

	UGZInventoryItemInstance* AddEntry(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);
	void RemoveEntry(UGZInventoryItemInstance* ItemInstance);
	void RemoveAllEntries();
	const FGZInventoryEntry* GetFirstEntryByEquipmentDefClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);
	//~FFastArraySerializer contract
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
	{
		UE_LOG(LogTemp, Warning, TEXT("FGZInventoryList::PostReplicatedAdd: %p, Items.Num(): %d"), this, Items.Num());
		OnPostReplicatedAdd.ExecuteIfBound(Items, AddedIndices, FinalSize);
	}

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
	{
		OnReplicatedRemove.ExecuteIfBound(Items, RemovedIndices, FinalSize);
	}

	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
	{
		UE_LOG(LogTemp, Warning, TEXT("FGZInventoryList::PostReplicatedChange: %p, Items.Num(): %d"), this, Items.Num());
		OnPostReplicatedChange.ExecuteIfBound(Items, ChangedIndices, FinalSize);
	}

	//~End of FFastArraySerializer contract
	FOnListReplicatedSingnature OnPostReplicatedAdd;
	FOnListReplicatedSingnature OnReplicatedRemove;
	FOnListReplicatedSingnature OnPostReplicatedChange;

	void SetOwnerComponent(UGZPawnComponent* Component)
	{
		OwnerComponent = Component;
	}

	const TArray<FGZInventoryEntry>& GetItems() const
	{
		return Items;
	}

	UPROPERTY()
	TArray<FGZInventoryEntry> Items;

private:
	//friend UGZInventoryManagerComponent;
	UPROPERTY(NotReplicated)
	TObjectPtr<UGZPawnComponent> OwnerComponent = nullptr;
};

//this is required for NetDeltaSerialize can be call
template <>
struct TStructOpsTypeTraits<FGZInventoryList> : public TStructOpsTypeTraitsBase2<FGZInventoryList>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTGZ_API UGZInventoryManagerComponent : public UGZPawnComponent
{
	GENERATED_BODY()

public:
	UGZInventoryManagerComponent();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void ReadyForReplication() override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UGZInventoryItemInstance* AddItemDefToInventory(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass);
	bool RemoveItemFromInventory(UGZInventoryItemInstance* ItemInstance);

	const TArray<FGZInventoryEntry>& GetEntries() const;

	FOnInventoryListItemUpdatedSingnature OnItemAdded;
	FOnInventoryListItemUpdatedSingnature OnItemWillRemove;
	FOnInventoryListItemUpdatedSingnature OnItemChanged;

protected:
	UPROPERTY(Replicated)
	FGZInventoryList InventoryList;

private:
	void OnPostReplicatedAdd(const TArray<FGZInventoryEntry>& ItemEntries, const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void OnReplicatedRemove(const TArray<FGZInventoryEntry>& ItemEntries, const TArrayView<int32>& RemovingIndices, int32 FinalSize);
	void OnPostReplicatedChange(const TArray<FGZInventoryEntry>& ItemEntries, const TArrayView<int32>& RemovedIndices, int32 FinalSize);
};

#pragma once

#include "CoreMinimal.h"
#include "GZInventoryItemInstance.h"
#include "Components/ActorComponent.h"
#include "GZInventoryManagerComponent.generated.h"

DECLARE_DELEGATE_TwoParams(FOnListReplicatedSingnature, const TArrayView<int32>&/*RemovedIndices*/, int32/*FinalSize*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryListItemUpdatedSingnature,UGZInventoryItemInstance*, ItemInstance);
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
	FGZInventoryList() : OwnerComponent(nullptr)
	{
	}

	FGZInventoryList(UActorComponent* OwnerComponent)
		: OwnerComponent(OwnerComponent)
	{
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FGZInventoryEntry, FGZInventoryList>(
			Items, DeltaParms, *this);
	}

	UGZInventoryItemInstance* AddEntry(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);
	void RemoveEntry(UGZInventoryItemInstance* ItemInstance);
	void RemoveAllEntries();
	const FGZInventoryEntry* GetEntryByEquipmentDefClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
	{
		OnReplicatedRemove.Execute(RemovedIndices, FinalSize);
	}

	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
	{
		OnPostReplicatedAdd.Execute(AddedIndices, FinalSize);
	}

	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
	{
		OnPostReplicatedChange.Execute(ChangedIndices, FinalSize);
	}

	//~End of FFastArraySerializer contract
	FOnListReplicatedSingnature OnPostReplicatedAdd;
	FOnListReplicatedSingnature OnReplicatedRemove;
	FOnListReplicatedSingnature OnPostReplicatedChange;

private:
	friend UGZInventoryManagerComponent;
	UPROPERTY()
	TArray<FGZInventoryEntry> Items;
	UPROPERTY()
	UActorComponent* OwnerComponent = nullptr;
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
class PROJECTGZ_API UGZInventoryManagerComponent : public UActorComponent
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
	void OnPostReplicatedAdd(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void OnReplicatedRemove(const TArrayView<int32>& RemovingIndices, int32 FinalSize);
	void OnPostReplicatedChange(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
};

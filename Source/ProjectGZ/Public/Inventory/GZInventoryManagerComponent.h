#pragma once

#include "CoreMinimal.h"
#include "GZInventoryItemInstance.h"
#include "Components/ActorComponent.h"
#include "GZInventoryManagerComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryListReplicatedSingnature, const TArrayView<int32>&/*RemovedIndices*/, int32/*FinalSize*/);

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
	FGZInventoryList() : OwnerComponent(nullptr)
	{
	}

	FGZInventoryList(UActorComponent* OwnerComponent)
		: OwnerComponent(OwnerComponent)
	{
	}

	GENERATED_BODY()
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
		OnInventoryListPreRemove.Broadcast(RemovedIndices, FinalSize);
	}

	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
	{
		OnInventoryListPostAdd.Broadcast(AddedIndices, FinalSize);
	}

	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
	{
		OnInventoryListPostChange.Broadcast(ChangedIndices, FinalSize);
	}

	//~End of FFastArraySerializer contract
	FOnInventoryListReplicatedSingnature OnInventoryListPostAdd;
	FOnInventoryListReplicatedSingnature OnInventoryListPreRemove;
	FOnInventoryListReplicatedSingnature OnInventoryListPostChange;

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

UENUM(BlueprintType)
enum class EInventoryListModifyAction:uint8
{
	ItemAdded UMETA(DisplayName="Some Items just Added"),
	ItemRemoved UMETA(DisplayName="Some Items wiil be Removed"),
	ListChanged UMETA(DisplayName="Part or whole list Changed"),
};

USTRUCT(BlueprintType)
struct FInventoryListModifyData
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly)
	TMap<int32, FGZInventoryEntry> ItemInstances;
	UPROPERTY(BlueprintReadOnly)
	int32 FinalSize;
	UPROPERTY(BlueprintReadOnly)
	EInventoryListModifyAction ModifyAction;
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
	void QueryInventoryDatas(FInventoryListModifyData& ModifyData);

protected:
	UPROPERTY(Replicated)
	FGZInventoryList InventoryList;

private:
	bool SetupListModifyData(const TArrayView<int32>& AddedIndices, int32 FinalSize, FInventoryListModifyData& ModifyData);
	void OnInventoryListPostAdd(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void OnInventoryListPreRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void OnInventoryListPostChange(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
};

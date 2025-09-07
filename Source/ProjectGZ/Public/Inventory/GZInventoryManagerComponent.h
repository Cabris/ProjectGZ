#pragma once
#include "CoreMinimal.h"
#include "GZInventoryItemInstance.h"
#include "Character/GZPawnComponent.h"
#include "Components/ActorComponent.h"
#include "GZInventoryManagerComponent.generated.h"

struct FGZInventoryEntry;
DECLARE_DELEGATE_ThreeParams(FOnListReplicatedSingnature, const TArray<FGZInventoryEntry>&, const TArrayView<int32>&/*RemovedIndices*/,
                             int32/*FinalSize*/);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryListItemUpdatedSingnature, UGZInventoryItemInstance*, ItemInstance);

class UGZInventoryManagerComponent;

USTRUCT(BlueprintType)
struct FGZInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	UGZInventoryItemInstance* GetItemInstance() const
	{
		return ItemInstance.Get();
	}

private:
	friend struct FGZInventoryList;
	//friend UGZInventoryManagerComponent;
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

	UGZInventoryItemInstance* AddEntry(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass, AActor* Outer);
	void RemoveEntry(UGZInventoryItemInstance* ItemInstance);
	void RemoveAllEntries();
	const FGZInventoryEntry* GetFirstEntryByEquipmentDefClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass);

	//~FFastArraySerializer contract
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	void SetOwnerComponent(UGZInventoryManagerComponent* Component)
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
	TObjectPtr<UGZInventoryManagerComponent> OwnerComponent = nullptr;
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

public:
	void OnPostReplicatedAdd(const TArray<FGZInventoryEntry>& ItemEntries, const TArrayView<int32>& RemovedIndices, int32 FinalSize);
};

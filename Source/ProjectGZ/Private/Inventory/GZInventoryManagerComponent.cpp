#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Engine/ActorChannel.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

bool FGZInventoryList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	bool bHasAuthority = OwnerComponent->GetOwner()->HasAuthority();
	// UE_LOG(LogTemp, Warning, TEXT("FGZInventoryList::NetDeltaSerialize: %p, Items.Num(): %d, bHasAuthority: %d"), this, Items.Num(),
	//        bHasAuthority);
	// 嘗試序列化
	bool result = FFastArraySerializer::FastArrayDeltaSerialize<FGZInventoryEntry, FGZInventoryList>(
		Items, DeltaParms, *this);
	// UE_LOG(LogTemp, Error, TEXT("FGZInventoryList::NetDeltaSerialize: Result: %s"), result ? TEXT("SUCCESS") : TEXT("FAILED"));
	return result;
}

UGZInventoryItemInstance* FGZInventoryList::AddEntry(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass, AActor* Outer)
{
	if (!IsValid(OwnerComponent.Get()))return nullptr;
	if (!IsValid(ItemDefClass))return nullptr;
	UGZInventoryItemDefinition* ItemDef = ItemDefClass.GetDefaultObject();
	if (!ItemDef)return nullptr;
	FGZInventoryEntry& NewEntry = Items.AddDefaulted_GetRef();
	NewEntry.ItemInstance = NewObject<UGZInventoryItemInstance>(Outer);
	NewEntry.ItemInstance->SetItemDefinitionClass(ItemDefClass);
	const TArray<FGameplayTagStackEntry>& ItemTagStacks = ItemDef->GetItemTagStacksRef();
	for (const FGameplayTagStackEntry& ItemTagStack : ItemTagStacks)
	{
		NewEntry.ItemInstance->SetStackByTag(ItemTagStack.Tag, ItemTagStack.Count);
	}
	MarkItemDirty(NewEntry);
	bool bHasAuthority = OwnerComponent->GetOwner()->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("FGZInventoryList::NetDeltaSerialize: %p, Items.Num(): %d, bHasAuthority: %d"), this, Items.Num(),
	       bHasAuthority);
	return NewEntry.ItemInstance.Get();
}

void FGZInventoryList::RemoveEntry(UGZInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance))return;

	const int32 Idx = Items.IndexOfByPredicate([ItemInstance](const FGZInventoryEntry& Entry)
	{
		return Entry.ItemInstance == ItemInstance;
	});
	if (Idx != INDEX_NONE)
	{
		Items.RemoveAtSwap(Idx); //order is not important
		MarkArrayDirty();
	}
}

void FGZInventoryList::RemoveAllEntries()
{
	if (Items.Num() > 0)
	{
		Items.Reset();
		MarkArrayDirty();
	}
}

const FGZInventoryEntry* FGZInventoryList::GetFirstEntryByEquipmentDefClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass)
{
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		FGZInventoryEntry& Entry = *It;
		if (Entry.ItemInstance != nullptr && Entry.ItemInstance->GetItemDefinitionClass() == ItemDefClass)
		{
			return &Entry;
		}
	}
	return nullptr;
}

void FGZInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	bool bHasAuthority = OwnerComponent->GetOwner()->HasAuthority(); //HasAuthority false
	UE_LOG(LogTemp, Warning, TEXT("FGZInventoryList::PostReplicatedAdd: %p, Items.Num(): %d, bHasAuthority: %d, FinalSize: %d"),
	       this, Items.Num(), bHasAuthority, FinalSize);
	for (int32 Idx : AddedIndices)
	{
		auto& Entry = Items[Idx];
		if (IsValid(Entry.ItemInstance))
		{
			OwnerComponent->OnItemAdded.Broadcast(Entry.GetItemInstance());
		}
	}
}

void FGZInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Idx : RemovedIndices)
	{
		auto& Entry = Items[Idx];
		if (IsValid(Entry.GetItemInstance()))
			OwnerComponent->OnItemWillRemove.Broadcast(Entry.GetItemInstance());
	}
}

void FGZInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Idx : ChangedIndices)
	{
		auto& Entry = Items[Idx];
		if (IsValid(Entry.GetItemInstance()))
			OwnerComponent->OnItemChanged.Broadcast(Entry.GetItemInstance());
	}
}

const TArray<FGZInventoryEntry>& UGZInventoryManagerComponent::GetEntries() const
{
	return InventoryList.GetItems();
}

UGZInventoryManagerComponent::UGZInventoryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	InventoryList.SetOwnerComponent(this);
}

void UGZInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, InventoryList);
}

void UGZInventoryManagerComponent::OnPostReplicatedAdd(const TArray<FGZInventoryEntry>& ItemEntries, const TArrayView<int32>& AddedIndices,
                                                       int32 FinalSize)
{
	bool bHasAuthority = GetOwner()->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("OnPostReplicatedAdd::HasAuthority: %d"), bHasAuthority);
	for (int32 Idx : AddedIndices)
	{
		auto& Entry = ItemEntries[Idx];
		if (IsValid(Entry.GetItemInstance()))
			OnItemAdded.Broadcast(Entry.GetItemInstance());
	}
}

//Called On Server
UGZInventoryItemInstance* UGZInventoryManagerComponent::AddItemDefToInventory(
	const TSubclassOf<UGZInventoryItemDefinition>& ItemDefinitionClass)
{
	if (!IsValid(ItemDefinitionClass)) return nullptr;
	const FGZInventoryEntry* existEntry = InventoryList.GetFirstEntryByEquipmentDefClass(ItemDefinitionClass);

	//only one instance per type
	/*if (existEntry && existEntry->ItemInstance != nullptr)
	{
		return existEntry->ItemInstance;
	}*/

	UGZInventoryItemInstance* Instance = InventoryList.AddEntry(ItemDefinitionClass, GetOwner());
	if (!Instance)return nullptr;
	if (IsReadyForReplication() && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(Instance);
	}
	OnItemAdded.Broadcast(Instance);
	return Instance;
}

//Called On Server
bool UGZInventoryManagerComponent::RemoveItemFromInventory(UGZInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance)) return false;
	InventoryList.RemoveEntry(ItemInstance);
	OnItemWillRemove.Broadcast(ItemInstance);
	if (IsReadyForReplication() && IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
	return true;
}

void UGZInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();
	if (IsUsingRegisteredSubObjectList())
	{
		const TArray<FGZInventoryEntry>& ItemEntries = GetEntries();
		for (const FGZInventoryEntry& Entry : ItemEntries)
		{
			UGZInventoryItemInstance* Instance = Entry.GetItemInstance();
			if (IsValid(Instance))
				AddReplicatedSubObject(Instance);
		}
	}
}

bool UGZInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	const TArray<FGZInventoryEntry>& ItemEntries = GetEntries();

	for (const FGZInventoryEntry& Entry : ItemEntries)
	{
		UGZInventoryItemInstance* Instance = Entry.GetItemInstance();
		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

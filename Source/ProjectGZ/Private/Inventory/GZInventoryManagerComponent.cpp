#include "Inventory/GZInventoryManagerComponent.h"
#include "Engine/ActorChannel.h"
#include "Game/GZGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

UGZInventoryItemInstance* FGZInventoryList::AddEntry(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass)
{
	if (!IsValid(ItemDefClass))return nullptr;
	UGZInventoryItemDefinition* ItemDef = ItemDefClass.GetDefaultObject();
	if (!ItemDef)return nullptr;
	FGZInventoryEntry& NewEntry = Items.AddDefaulted_GetRef();
	NewEntry.ItemInstance = NewObject<UGZInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.ItemInstance->SetItemDefinitionClass(ItemDefClass);
	const TArray<FGameplayTagStackEntry>& ItemTagStacks = ItemDef->GetItemTagStacksRef();
	for (const FGameplayTagStackEntry& ItemTagStack : ItemTagStacks)
	{
		NewEntry.ItemInstance->SetStackByTag(ItemTagStack.Tag, ItemTagStack.Count);
	}

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
		Items.RemoveAtSwap(Idx);//order is not important
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

const TArray<FGZInventoryEntry>& UGZInventoryManagerComponent::GetEntries() const
{
	return InventoryList.Items;
}

UGZInventoryManagerComponent::UGZInventoryManagerComponent(): InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	InventoryList.OnReplicatedRemove.BindUObject(this, &ThisClass::OnReplicatedRemove);
	InventoryList.OnPostReplicatedAdd.BindUObject(this, &ThisClass::OnPostReplicatedAdd);
	InventoryList.OnPostReplicatedChange.BindUObject(this, &ThisClass::OnPostReplicatedChange);
}

void UGZInventoryManagerComponent::OnPostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 Idx : AddedIndices)
	{
		auto& Entry = InventoryList.Items[Idx];
		if (IsValid(Entry.ItemInstance))
			OnItemAdded.Broadcast(Entry.ItemInstance);
	}
}

void UGZInventoryManagerComponent::OnReplicatedRemove(const TArrayView<int32>& RemovingIndices, int32 FinalSize)
{
	for (int32 Idx : RemovingIndices)
	{
		auto& Entry = InventoryList.Items[Idx];
		if (IsValid(Entry.ItemInstance))
			OnItemWillRemove.Broadcast(Entry.ItemInstance);
	}
}

void UGZInventoryManagerComponent::OnPostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 Idx : ChangedIndices)
	{
		auto& Entry = InventoryList.Items[Idx];
		if (IsValid(Entry.ItemInstance))
			OnItemChanged.Broadcast(Entry.ItemInstance);
	}
}

void UGZInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, InventoryList);
}

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

	UGZInventoryItemInstance* Instance = InventoryList.AddEntry(ItemDefinitionClass);
	if (!Instance)return nullptr;
	if (IsReadyForReplication() && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(Instance);
	}
	OnItemAdded.Broadcast(Instance);
	return Instance;
}

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
		for (FGZInventoryEntry& Entry : InventoryList.Items)
		{
			UGZInventoryItemInstance* Instance = Entry.ItemInstance;
			if (IsValid(Instance))
				AddReplicatedSubObject(Instance);
		}
	}
}

bool UGZInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FGZInventoryEntry& Entry : InventoryList.Items)
	{
		UGZInventoryItemInstance* Instance = Entry.ItemInstance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

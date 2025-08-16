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
	auto& ItemTagStacks = ItemDef->GetItemTagStacks();
	for (auto& ItemTagStack : ItemTagStacks)
	{
		NewEntry.ItemInstance->SetStackByTag(ItemTagStack.Key, ItemTagStack.Value);
	}

	return NewEntry.ItemInstance;
}

void FGZInventoryList::RemoveEntry(UGZInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance))return;
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		FGZInventoryEntry& Entry = *It;
		if (Entry.ItemInstance == ItemInstance)
		{
			It.RemoveCurrent();
			MarkArrayDirty();
			return;
		}
	}
}

void FGZInventoryList::RemoveAllEntries()
{
	bool HasRemove = false;
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		FGZInventoryEntry& Entry = *It;
		if (Entry.ItemInstance != nullptr)
		{
			HasRemove = true;
			It.RemoveCurrent();
		}
	}
	if (HasRemove)
		MarkArrayDirty();
}

const FGZInventoryEntry* FGZInventoryList::GetEntryByEquipmentDefClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass)
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

UGZInventoryManagerComponent::UGZInventoryManagerComponent(): InventoryList(this)
{
	PrimaryComponentTick.bCanEverTick = false;
	InventoryList.OnInventoryListPreRemove.AddUObject(this, &ThisClass::OnInventoryListPreRemove);
	InventoryList.OnInventoryListPostAdd.AddUObject(this, &ThisClass::OnInventoryListPostAdd);
	InventoryList.OnInventoryListPostChange.AddUObject(this, &ThisClass::OnInventoryListPostChange);
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
	const FGZInventoryEntry* existEntry = InventoryList.GetEntryByEquipmentDefClass(ItemDefinitionClass);
	if (existEntry && existEntry->ItemInstance != nullptr)
	{
		return existEntry->ItemInstance;
	}
	UGZInventoryItemInstance* Instance = InventoryList.AddEntry(ItemDefinitionClass);
	if (!Instance)return nullptr;
	if (IsReadyForReplication() && IsUsingRegisteredSubObjectList())
	{
		AddReplicatedSubObject(Instance);
	}
	return Instance;
}

void UGZInventoryManagerComponent::QueryInventoryDatas(FInventoryListModifyData& ModifyData)
{
	ModifyData.ModifyAction = EInventoryListModifyAction::ListChanged;
	ModifyData.FinalSize = InventoryList.Items.Num();
	ModifyData.ItemInstances.Reserve(InventoryList.Items.Num());
	for (int32 ItemIndex = 0; ItemIndex < InventoryList.Items.Num(); ++ItemIndex)
	{
		ModifyData.ItemInstances[ItemIndex] = InventoryList.Items[ItemIndex];
	}
}

bool UGZInventoryManagerComponent::SetupListModifyData(const TArrayView<int32>& AddedIndices, int32 FinalSize, FInventoryListModifyData& ModifyData)
{
	ModifyData.FinalSize = FinalSize;
	ModifyData.ItemInstances.Reserve(AddedIndices.Num());
	for (int32 Indices : AddedIndices)
	{
		if (!(Indices >= 0 && Indices < InventoryList.Items.Num()))
		{
			UE_LOG(LogTemp, Error, TEXT("Indices: %d, invalid"), Indices);
			return false;
		}
		ModifyData.ItemInstances[Indices] = InventoryList.Items[Indices];
	}
	return true;
}

void UGZInventoryManagerComponent::OnInventoryListPostAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	FInventoryListModifyData ModifyData;
	ModifyData.ModifyAction = EInventoryListModifyAction::ItemAdded;
	if (!SetupListModifyData(AddedIndices, FinalSize, ModifyData)) return;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(GZGameplayTags::MessageTag_Inventory_Changed, ModifyData);
}

void UGZInventoryManagerComponent::OnInventoryListPreRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	FInventoryListModifyData ModifyData;
	ModifyData.ModifyAction = EInventoryListModifyAction::ItemRemoved;
	if (!SetupListModifyData(RemovedIndices, FinalSize, ModifyData)) return;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(GZGameplayTags::MessageTag_Inventory_Changed, ModifyData);
}

void UGZInventoryManagerComponent::OnInventoryListPostChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	FInventoryListModifyData ModifyData;
	ModifyData.ModifyAction = EInventoryListModifyAction::ListChanged;
	if (!SetupListModifyData(ChangedIndices, FinalSize, ModifyData)) return;
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(GZGameplayTags::MessageTag_Inventory_Changed, ModifyData);
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

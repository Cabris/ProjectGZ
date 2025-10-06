#include "Equipment/GZTagStackList.h"

void FGZTagStackList::AddItemStack(FGameplayTag Tag, int32 StackCount)
{
	if (StackCount <= 0)return;
	if (!ContainsItemStack(Tag))return;
	for (FGZTagStackEntry Entry : Items)
	{
		if (Entry.ItemTag == Tag)
		{
			Entry.StackCount += StackCount;
			ItemStack[Tag] = Entry.StackCount;
			MarkItemDirty(Entry);
			return;
		}
	}
}

void FGZTagStackList::RemoveItem(FGameplayTag Tag)
{
	if (!ContainsItemStack(Tag))return;
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		FGZTagStackEntry& Entry = *It;
		if (Entry.ItemTag == Tag)
		{
			It.RemoveCurrentSwap();
			ItemStack.Remove(Tag);
			MarkArrayDirty();
			return;
		}
	}
}

void FGZTagStackList::RemoveItemStack(FGameplayTag Tag, int32 StackCount)
{
	if (StackCount <= 0)return;
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		FGZTagStackEntry& Entry = *It;
		if (Entry.ItemTag == Tag)
		{
			Entry.StackCount -= StackCount;
			ItemStack[Tag] = Entry.StackCount;
			MarkItemDirty(Entry);
			return;
		}
	}
}

void FGZTagStackList::SetItemStackCount(FGameplayTag Tag, int32 StackCount)
{
	//Create NewEntry
	if (!ContainsItemStack(Tag))
	{
		FGZTagStackEntry& NewEntry = Items.AddDefaulted_GetRef();
		NewEntry.ItemTag = Tag;
		NewEntry.StackCount = StackCount;
		ItemStack.Add(Tag, StackCount);
		MarkItemDirty(NewEntry);
	}
	else
	{
		for (auto It = Items.CreateIterator(); It; ++It)
		{
			FGZTagStackEntry& Entry = *It;
			if (Entry.ItemTag == Tag)
			{
				Entry.StackCount = StackCount;
				ItemStack[Tag] = Entry.StackCount;
				MarkItemDirty(Entry);
				return;
			}
		}
	}
}

void FGZTagStackList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FGZTagStackEntry& EntryToRemove = Items[Index];
		ItemStack.Remove(EntryToRemove.ItemTag);
	}
}

void FGZTagStackList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FGZTagStackEntry& EntryToAdd = Items[Index];
		ItemStack.Add(EntryToAdd.ItemTag, EntryToAdd.StackCount);
	}
}

void FGZTagStackList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FGZTagStackEntry& EntryToChange = Items[Index];
		ItemStack[EntryToChange.ItemTag] = EntryToChange.StackCount;
	}
}

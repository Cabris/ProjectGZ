#include "Equipment/GZTagStackList.h"

void FGZTagStackList::AddItemStack(FGameplayTag Tag, int32 StackCount)
{
	if (StackCount <= 0)return;
	for (FGZTagStackEntry Entry : Items)
	{
		if (Entry.ItemTag == Tag)
		{
			Entry.StackCount += StackCount;
			ItemStack[Tag] = Entry.StackCount;
			MarkArrayDirty();
			return;
		}
	}
	FGZTagStackEntry& NewEntry = Items.AddDefaulted_GetRef();
	NewEntry.ItemTag = Tag;
	NewEntry.StackCount = StackCount;
	ItemStack.Add(Tag, StackCount);
	MarkArrayDirty();
}

void FGZTagStackList::RemoveItemStack(FGameplayTag Tag, int32 StackCount)
{
	if (StackCount <= 0)return;
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		FGZTagStackEntry& Entry = *It;
		if (Entry.ItemTag == Tag)
		{
			if (Entry.StackCount <= StackCount) //delete entry
			{
				It.RemoveCurrentSwap();
				ItemStack.Remove(Tag);
				MarkArrayDirty();
				return;
			}
			Entry.StackCount -= StackCount;
			ItemStack[Tag] = Entry.StackCount;
			MarkArrayDirty();
			return;
		}
	}
}

void FGZTagStackList::SetItemStackCount(FGameplayTag Tag, int32 StackCount)
{
	for (auto It = Items.CreateIterator(); It; ++It)
	{
		FGZTagStackEntry& Entry = *It;
		if (Entry.ItemTag == Tag)
		{
			Entry.StackCount = StackCount;
			ItemStack[Tag] = Entry.StackCount;
			MarkArrayDirty();
			return;
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

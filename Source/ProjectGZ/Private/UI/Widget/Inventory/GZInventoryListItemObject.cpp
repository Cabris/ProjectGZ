#include "UI/Widget/Inventory/GZInventoryListItemObject.h"

#include "Inventory/GZInventoryItemInstance.h"

void UGZInventoryListItemObject::InitializeWithItemInstance(UGZInventoryItemInstance* InItemInstance, int32 InArrayIndex)
{
	ItemInstance = InItemInstance;
	ArrayIndex = InArrayIndex;
}

UGZInventoryItemDefinition* UGZInventoryListItemObject::GetItemDefinition() const
{
	if (!IsValid(ItemInstance))
	{
		return nullptr;
	}

	const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass = ItemInstance->GetItemDefinitionClass();
	return IsValid(ItemDefClass) ? ItemDefClass.GetDefaultObject() : nullptr;
}

FName UGZInventoryListItemObject::GetItemName() const
{
	if (UGZInventoryItemDefinition* ItemDef = GetItemDefinition())
	{
		return ItemDef->GetItemName();
	}
	return FName();
}

UTexture2D* UGZInventoryListItemObject::GetItemIcon() const
{
	if (UGZInventoryItemDefinition* ItemDef = GetItemDefinition())
	{
		return ItemDef->GetItemIcon().Get();
	}
	return nullptr;
}

int32 UGZInventoryListItemObject::GetItemQuantity(const FGameplayTag& QuantityTag) const
{
	if (!IsValid(ItemInstance) || !QuantityTag.IsValid())
	{
		return 0;
	}

	return static_cast<int32>(ItemInstance->GetStackByTag(QuantityTag));
}
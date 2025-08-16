#include "Inventory/GZInventoryItemDefinition.h"

const TObjectPtr<UTexture2D>& UGZInventoryItemDefinition::GetItemIcon() const
{
	return ItemIcon;
}

const TSubclassOf<UGZEquipmentDefinition>& UGZInventoryItemDefinition::GetEquipmentDef() const
{
	return EquipmentDef;
}

const FName& UGZInventoryItemDefinition::GetItemName() const
{
	return ItemName;
}

const TSubclassOf<UGZInventoryItemInstance>& UGZInventoryItemDefinition::GetItemInstanceeDef() const
{
	return InstanceDef;
}

const TMap<FGameplayTag, uint32>& UGZInventoryItemDefinition::GetItemTagStacks() const
{
	return ItemTagStacks;
}

const TObjectPtr<UStaticMesh>& UGZInventoryItemDefinition::GetPickupMesh() const
{
	return PickupMesh;
}

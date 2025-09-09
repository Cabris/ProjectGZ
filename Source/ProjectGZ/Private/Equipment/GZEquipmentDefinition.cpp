#include "Equipment/GZEquipmentDefinition.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "ProjectGZ/ProjectGZ.h"


UGZEquipmentDefinition* UGZEquipmentDefinition::GetDefinition(UGZInventoryItemInstance* ItemInstance)
{
	if (!IsValid(ItemInstance)) return nullptr;
	UGZInventoryItemDefinition* ItemDefinition = ItemInstance->GetItemDefinition();
	if (!IsValid(ItemDefinition)) return nullptr;
	TSubclassOf<UGZEquipmentDefinition> EquipmentDefClass = ItemDefinition->GetEquipmentDef();
	if (!IsValid(EquipmentDefClass)) return nullptr;

	//can be equipped
	UGZEquipmentDefinition* EquipmentDef = EquipmentDefClass.GetDefaultObject();
	if (!EquipmentDef || !IsValid(EquipmentDef->InstanceClass))
	{
		DEBUG_PRINTF(TEXT("Failed to TryGrantEquipmentToPawn: EquipmentDef is NULL"));
		return nullptr;
	}
	return EquipmentDef;
}


#include "Inventory/GZInventoryItemInstance.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

int32 UGZInventoryItemInstance::GetStackByTag(const FGameplayTag& Tag) const
{
	return TagStackList.GetItemStackCount(Tag);
}

void UGZInventoryItemInstance::SetStackByTag(const FGameplayTag& Tag, int32 Stack)
{
	TagStackList.SetItemStackCount(Tag, Stack);
}

TSubclassOf<UGZInventoryItemDefinition> UGZInventoryItemInstance::GetItemDefinitionClass() const
{
	return ItemDefinitionClass;
}

UGZInventoryItemDefinition* UGZInventoryItemInstance::GetItemDefinition() const
{
	if (!IsValid(ItemDefinitionClass)) return nullptr;
	return ItemDefinitionClass.GetDefaultObject();
}

void UGZInventoryItemInstance::SetItemDefinitionClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass)
{
	ItemDefinitionClass = ItemDefClass;
}

void UGZInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, ItemDefinitionClass);
	DOREPLIFETIME(ThisClass, TagStackList);
}

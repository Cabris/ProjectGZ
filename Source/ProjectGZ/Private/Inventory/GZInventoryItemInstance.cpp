#include "Inventory/GZInventoryItemInstance.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

uint32 UGZInventoryItemInstance::GetStackByTag(const FGameplayTag& Tag) const
{
	return TagStackList.GetItemStackCount(Tag);
}

void UGZInventoryItemInstance::SetStackByTag(const FGameplayTag& Tag, int32 Stack)
{
	TagStackList.SetItemStackCount(Tag, Stack);
}

const TSubclassOf<UGZInventoryItemDefinition>& UGZInventoryItemInstance::GetItemDefinitionClass() const
{
	return ItemDefinitionClass;
}

void UGZInventoryItemInstance::SetItemDefinitionClass(const TSubclassOf<UGZInventoryItemDefinition>& ItemDefClass)
{
	ItemDefinitionClass = ItemDefClass;
}

void UGZInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass,ItemDefinitionClass);
	DOREPLIFETIME(ThisClass,TagStackList);
}

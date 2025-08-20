#include "Inventory/GZInventoryItemInstance.h"

#include "Game/GZGameplayTags.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

int32 UGZInventoryItemInstance::GetStackByTag(FGameplayTag Tag) const
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

int32 UGZInventoryItemInstance::GetDefaultStack() const
{
	return TagStackList.GetItemStackCount(GZGameplayTags::Item_Stack_Quantity);
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

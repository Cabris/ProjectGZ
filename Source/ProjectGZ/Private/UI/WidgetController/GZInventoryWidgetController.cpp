#include "UI/WidgetController/GZInventoryWidgetController.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZPawnFeatureComponent.h"

void UGZInventoryWidgetController::BroadcastInitialValues()
{
	const TObjectPtr<UGZInventoryManagerComponent>& InventoryManager = PawnFeatureComponent->GetInventoryManager();
	if (!IsValid(InventoryManager.Get()))return;
	const TArray<FGZInventoryEntry>& Entries = InventoryManager->GetEntries();
	TrackItems.Empty();
	TrackItems.Reserve(Entries.Num());
	for (int32 i = 0; i < Entries.Num(); i++)
	{
		TrackItems.Add(Entries[i].GetItemInstance());
	}

	OnItemListInitialized.ExecuteIfBound(TrackItems);
}

void UGZInventoryWidgetController::BindCallbacksToDependencies()
{
	const TObjectPtr<UGZInventoryManagerComponent>& InventoryManager = PawnFeatureComponent->GetInventoryManager();
	if (!IsValid(InventoryManager.Get()))return;
	InventoryManager->OnItemAdded.AddDynamic(this, &ThisClass::HandleItemAdded);
	InventoryManager->OnItemWillRemove.AddDynamic(this, &ThisClass::HandleItemWillRemove);
	InventoryManager->OnItemChanged.AddDynamic(this, &ThisClass::HandleItemChanged);
}

void UGZInventoryWidgetController::RefreshInventory()
{
}

void UGZInventoryWidgetController::ClearAllItems()
{
}

void UGZInventoryWidgetController::HandleItemAdded(UGZInventoryItemInstance* ItemInstance)
{
	if (TrackItems.Contains(ItemInstance))return;
	TrackItems.Add(ItemInstance);
	OnItemAdded.ExecuteIfBound(ItemInstance);
}

void UGZInventoryWidgetController::HandleItemWillRemove(UGZInventoryItemInstance* ItemInstance)
{
	if (!TrackItems.Contains(ItemInstance))return;
	TrackItems.Remove(ItemInstance);
	OnItemWillRemove.ExecuteIfBound(ItemInstance);
}

void UGZInventoryWidgetController::HandleItemChanged(UGZInventoryItemInstance* ItemInstance)
{
	OnItemChanged.ExecuteIfBound(ItemInstance);
}

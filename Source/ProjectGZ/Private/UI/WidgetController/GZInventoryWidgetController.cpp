#include "UI/WidgetController/GZInventoryWidgetController.h"
#include "Game/GZGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Character/GZPawnFeatureComponent.h"
#include "GameplayTagContainer.h"

void UGZInventoryWidgetController::BroadcastInitialValues()
{
	TObjectPtr<UGZInventoryManagerComponent> InventoryManager = PawnFeatureComponent->GetInventoryManager();
	if (!IsValid(InventoryManager))return;
	FInventoryListModifyData ModifyData;
	ModifyData.ModifyAction= EInventoryListModifyAction::ListChanged;
	InventoryManager->QueryInventoryDatas(ModifyData);
	OnInventoryListUpdated.Broadcast(ModifyData);
}

void UGZInventoryWidgetController::BindCallbacksToDependencies()
{
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.RegisterListener<FInventoryListModifyData>(GZGameplayTags::MessageTag_Inventory_Changed, this, &ThisClass::OnInventoryModified);
}

void UGZInventoryWidgetController::OnInventoryModified(FGameplayTag Channel, const FInventoryListModifyData& ModifyData)
{
	OnInventoryListUpdated.Broadcast(ModifyData);
}

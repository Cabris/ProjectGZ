#pragma once
#include "CoreMinimal.h"
#include "GZWidgetController.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "GZInventoryWidgetController.generated.h"

DECLARE_DELEGATE_OneParam(FOnItemInstanceUpdatedSingnature, UGZInventoryItemInstance* ItemInstance)
DECLARE_DELEGATE_OneParam(FOnItemListInitializedSingnature, const TArray<UGZInventoryItemInstance*>& ItemList)

UCLASS()
class PROJECTGZ_API UGZInventoryWidgetController : public UGZWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	void RefreshInventory();
	void ClearAllItems();
	FOnItemInstanceUpdatedSingnature OnItemAdded;
	FOnItemInstanceUpdatedSingnature OnItemWillRemove;
	FOnItemInstanceUpdatedSingnature OnItemChanged;
	FOnItemListInitializedSingnature OnItemListInitialized;
protected:
	UFUNCTION()
	void HandleItemAdded(UGZInventoryItemInstance* ItemInstance);
	UFUNCTION()
	void HandleItemWillRemove(UGZInventoryItemInstance* ItemInstance);
	UFUNCTION()
	void HandleItemChanged(UGZInventoryItemInstance* ItemInstance);
	void HandleItemListChanged(TArray<UGZInventoryItemInstance*>& ItemList);
	UPROPERTY(Transient)
	TArray<UGZInventoryItemInstance*> TrackItems;
};

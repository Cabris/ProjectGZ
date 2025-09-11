#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GZInventoryEntryWidget.h"
#include "GZUserWidget.h"
#include "Components/TileView.h"
#include "Game/GZObjectPool.h"
#include "GZInventoryWidget.generated.h"

class UListView;

UCLASS()
class PROJECTGZ_API UGZInventoryWidget : public UGZUserWidget
{
	GENERATED_BODY()

public:
	UGZInventoryWidget();
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestRefreshInventory();
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestClearAllItems();

protected:
	//Called after NativePreConstruct, before Tick
	virtual void NativeConstruct() override;
	//Called after RemoveFromParent, Last step in Destruction
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryWidgetInitialized();
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryItemAdded(UGZInventoryItemInstance* ItemInstance);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryItemWillRemove(UGZInventoryItemInstance* ItemInstance);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryListCompletelyChanged();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Settings")
	FGameplayTag DefaultQuantityTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Settings")
	bool bEnableAutoSort = false;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UListView> ListView;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UGZInventoryEntryWidget> EntryWidgetClass;

private:
	void BindToController();
	void UnbindFromController();
	void HandleItemListInitialized(const TArray<UGZInventoryItemInstance*>& ItemList);
	void HandleItemAdded(UGZInventoryItemInstance* ItemInstance);
	void HandleItemWillRemove(UGZInventoryItemInstance* ItemInstance);
	UFUNCTION()
	void HandleItemChanged(UGZInventoryItemInstance* ItemInstance);
	bool bIsBoundToController = false;
};

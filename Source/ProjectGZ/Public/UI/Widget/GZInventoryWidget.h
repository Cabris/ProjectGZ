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

	/** 手動重新整理背包顯示 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestRefreshInventory();

	/** 清空所有顯示項目 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RequestClearAllItems();
 
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Widget Controller 設定完成後的回調 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryWidgetInitialized();
	/** 背包項目新增的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryItemAdded(UGZInventoryItemInstance* ItemInstance);

	/** 背包項目移除的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryItemWillRemove(UGZInventoryItemInstance* ItemInstance);

	/** 背包清單完全更新的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryListCompletelyChanged();

	/** 預設數量標籤 (用於顯示物品數量) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory|Settings")
	FGameplayTag DefaultQuantityTag;

	/** 是否啟用自動排序 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Settings")
	bool bEnableAutoSort = false;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (BindWidget))
	TObjectPtr<UListView> ListView;
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UGZInventoryEntryWidget> EntryWidgetClass;
 
private:

	/** 綁定控制器事件 */
	void BindToController();

	/** 解綁控制器事件 */
	void UnbindFromController();
	
	/** 重建完整的項目清單 */
	void HandleItemListInitialized(const TArray<UGZInventoryItemInstance*>& ItemList);
	
	/** 處理項目新增 */
	void HandleItemAdded(UGZInventoryItemInstance* ItemInstance);

	/** 處理項目移除 */
	void HandleItemWillRemove(UGZInventoryItemInstance* ItemInstance);

	/** 處理項目變更 */
	UFUNCTION()
	void HandleItemChanged(UGZInventoryItemInstance* ItemInstance);

	/** 是否已經綁定到控制器 */
	bool bIsBoundToController = false;
};

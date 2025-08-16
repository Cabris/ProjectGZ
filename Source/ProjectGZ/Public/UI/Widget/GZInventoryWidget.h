#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GZInventoryEntryWidget.h"
#include "GZUserWidget.h"
#include "Game/GZObjectPool.h"
#include "GZInventoryWidget.generated.h"

struct FInventoryListModifyData;
struct FGZInventoryEntry;
class UListView;

UCLASS()
class PROJECTGZ_API UGZInventoryWidget : public UGZUserWidget
{
	GENERATED_BODY()

public:
	UGZInventoryWidget();
	/** 於BP的PreConstruct Event中呼叫 */
	UFUNCTION(BlueprintCallable)
	void SetListView(UListView* ListView);

	/** 手動重新整理背包顯示 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefreshInventoryDisplay();

	/** 清空所有顯示項目 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearAllItems();

	/** 取得當前顯示的項目數量 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetDisplayedItemCount() const;

	/** 設定物件池配置 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Pool")
	void SetPoolConfig(const FGZObjectPoolConfig& NewConfig);

	/** 獲取物件池統計資訊 */
	UFUNCTION(BlueprintPure, Category = "Inventory|Pool")
	void GetPoolStats(int32& OutAvailable, int32& OutTotal, int32& OutActive) const;

	/** 預熱物件池 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Pool")
	void WarmUpPool(int32 Count = 10);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** Widget Controller 設定完成後的回調 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryWidgetInitialized();
	/** 背包項目新增的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryItemsAdded(const TArray<UGZInventoryListItemObject*>& AddedItems);

	/** 背包項目移除的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryItemsRemoved(const TArray<UGZInventoryListItemObject*>& RemovedItems);

	/** 背包清單完全更新的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnInventoryListCompletelyChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnPoolObjectCreated(UObject* CreatedObject);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPoolObjectReturned(UObject* ReturnedObject);

	/** 預設數量標籤 (用於顯示物品數量) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Settings")
	FGameplayTag DefaultQuantityTag;

	/** 是否啟用自動排序 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Settings")
	bool bEnableAutoSort = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UListView> ListView;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGZInventoryEntryWidget> EntryWidgetClass;

	/** 物件池配置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Pool")
	FGZObjectPoolConfig PoolConfig;

private:
	/** 綁定控制器事件 */
	void BindToController();

	/** 解綁控制器事件 */
	void UnbindFromController();

	/** 背包列表變更回調 */
	UFUNCTION()
	void OnInventoryListUpdated(const FInventoryListModifyData& ModifyData);

	/** 重建完整的項目清單 */
	void RebuildCompleteItemList(const FInventoryListModifyData& ModifyData);

	/** 處理項目新增 */
	void HandleItemsAdded(const FInventoryListModifyData& ModifyData);

	/** 處理項目移除 */
	void HandleItemsRemoved(const FInventoryListModifyData& ModifyData);

	/** 建立列表項目物件 */
	UGZInventoryListItemObject* CreateListItemObject(const FGZInventoryEntry& Entry, int32 ArrayIndex);

	/** 對項目清單進行排序 */
	void SortItemList(TArray<UGZInventoryListItemObject*>& ItemList);

	/** 當前顯示的項目列表 */
	UPROPERTY(Transient)
	TArray<UGZInventoryListItemObject*> CurrentDisplayItems;

	/** 是否已經綁定到控制器 */
	bool bIsBoundToController = false;

	/** 初始化物件池 */
	void InitializeObjectPool();

	/** 從物件池獲取項目物件 */
	UGZInventoryListItemObject* GetFromPool();

	/** 將項目物件返回到池中 */
	void ReturnToPool(UGZInventoryListItemObject* Object);

	/** 物件池實例 */
	UPROPERTY(Transient)
	TObjectPtr<UGZObjectPool> ObjectPool;

	/** 是否已初始化物件池 */
	bool bPoolInitialized = false;
};

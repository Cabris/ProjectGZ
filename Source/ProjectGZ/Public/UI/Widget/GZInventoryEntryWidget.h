#pragma once

#include "CoreMinimal.h"
#include "GZUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "GZInventoryEntryWidget.generated.h"

struct FStreamableHandle;
class UTextBlock;
class UImage;
class UGZInventoryItemInstance;
class UGZInventoryItemDefinition;

UCLASS()
class PROJECTGZ_API UGZInventoryEntryWidget : public UGZUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UGZInventoryEntryWidget();

	/** 取得當前綁定的項目資料 */
	UFUNCTION(BlueprintPure, Category = "Inventory Entry")
	UGZInventoryItemInstance* GetListItemObject() const { return CachedItemInstance; }

	/** 取得物品實例 */
	UFUNCTION(BlueprintPure, Category = "Inventory Entry")
	UGZInventoryItemInstance* GetItemInstance() const;

	/** 取得物品定義 */
	UFUNCTION(BlueprintPure, Category = "Inventory Entry")
	UGZInventoryItemDefinition* GetItemDefinition() const;

	/** 手動更新顯示 */
	UFUNCTION(BlueprintCallable, Category = "Inventory Entry")
	void UpdateDisplay();
 
protected:
	// IUserObjectListEntry 介面實作
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnItemExpansionChanged(bool bIsExpanded) override;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	/** 當項目資料設定時的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Entry")
	void OnItemDataSet(UGZInventoryItemInstance* ItemData);

	/** 當選擇狀態改變時的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Entry")
	void OnSelectionStateChanged(bool bIsSelected);

	/** 當展開狀態改變時的 BP 事件 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Entry")
	void OnExpansionStateChanged(bool bIsExpanded);

	/** 物品圖標 Image (可選，由設計師在 BP 中綁定) */
	//有相同類型和確切名稱
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Entry|Components", meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;

	/** 物品名稱 TextBlock (可選，由設計師在 BP 中綁定) */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Entry|Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName = nullptr;

	/** 物品數量 TextBlock (可選，由設計師在 BP 中綁定) */
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Entry|Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemQuantity = nullptr;

	/** 預設數量標籤 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Entry|Settings")
	FGameplayTag QuantityTag;

	/** 是否顯示數量 (當數量為1時可能不顯示) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Entry|Settings")
	bool bShowQuantityWhenOne = false;

	/** 預設物品圖標 (當物品沒有圖標時使用) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Entry|Settings")
	TObjectPtr<UTexture2D> DefaultItemIcon = nullptr;

private:

	/** 更新物品圖標 */
	void UpdateItemIcon();

	/** 更新物品名稱 */
	void UpdateItemName();

	/** 更新物品數量 */
	void UpdateItemQuantity();
	
	/** 快取的項目資料物件 */
	UPROPERTY(Transient)
	TObjectPtr<UGZInventoryItemInstance> CachedItemInstance = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UGZInventoryItemDefinition> CachedItemDefinition = nullptr;
 
	/** 當前選擇狀態 */
	bool bIsCurrentlySelected = false;

	/** 當前展開狀態 */
	bool bIsCurrentlyExpanded = false;
};

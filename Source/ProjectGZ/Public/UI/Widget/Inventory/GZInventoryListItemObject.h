#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GZInventoryListItemObject.generated.h"

class UGZInventoryItemInstance;
class UGZInventoryItemDefinition;

UCLASS()
class PROJECTGZ_API UGZInventoryListItemObject : public UObject
{
	GENERATED_BODY()
public:
	/** 初始化資料物件 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void InitializeWithItemInstance(UGZInventoryItemInstance* InItemInstance, int32 InArrayIndex);

	/** 取得物品實例 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UGZInventoryItemInstance* GetItemInstance() const { return ItemInstance; }

	/** 取得陣列索引 */
	UFUNCTION(BlueprintPure, Category = "Inventory") 
	int32 GetArrayIndex() const { return ArrayIndex; }

	/** 取得物品定義 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UGZInventoryItemDefinition* GetItemDefinition() const;

	/** 取得物品名稱 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	FName GetItemName() const;

	/** 取得物品圖標 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UTexture2D* GetItemIcon() const;

	/** 取得物品數量 (根據指定標籤) */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetItemQuantity(const FGameplayTag& QuantityTag) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UGZInventoryItemInstance> ItemInstance = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 ArrayIndex = -1;
};

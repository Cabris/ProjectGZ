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

	UFUNCTION(BlueprintPure, Category = "Inventory Entry")
	UGZInventoryItemInstance* GetListItemObject() const { return CachedItemInstance; }

	UFUNCTION(BlueprintPure, Category = "Inventory Entry")
	UGZInventoryItemInstance* GetItemInstance() const;

	UFUNCTION(BlueprintPure, Category = "Inventory Entry")
	UGZInventoryItemDefinition* GetItemDefinition() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory Entry")
	void UpdateDisplay();

protected:
	// IUserObjectListEntry
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnItemExpansionChanged(bool bIsExpanded) override;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Entry")
	void OnItemDataSet(UGZInventoryItemInstance* ItemData);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Entry")
	void OnSelectionStateChanged(bool bIsSelected);
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory Entry")
	void OnExpansionStateChanged(bool bIsExpanded);

	//有相同類型和確切名稱
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Entry|Components", meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon = nullptr;
	//物品名稱 TextBlock
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Entry|Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName = nullptr;
	// 物品數量
	UPROPERTY(BlueprintReadOnly, Category = "Inventory Entry|Components", meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemQuantity = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Entry|Settings")
	FGameplayTag QuantityTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Entry|Settings")
	bool bShowQuantityWhenOne = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory Entry|Settings")
	TObjectPtr<UTexture2D> DefaultItemIcon = nullptr;

private:
	void UpdateItemIcon();
	void UpdateItemName();
	void UpdateItemQuantity();

	UPROPERTY(Transient)
	TObjectPtr<UGZInventoryItemInstance> CachedItemInstance = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UGZInventoryItemDefinition> CachedItemDefinition = nullptr;

	bool bIsCurrentlySelected = false;
	bool bIsCurrentlyExpanded = false;
};

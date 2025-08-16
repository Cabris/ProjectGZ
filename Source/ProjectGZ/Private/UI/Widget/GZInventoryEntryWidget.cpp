#include "UI/Widget/GZInventoryEntryWidget.h"
#include "UI/Widget/GZInventoryWidget.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "UI/Widget/Inventory/GZInventoryListItemObject.h"

UGZInventoryEntryWidget::UGZInventoryEntryWidget()
{
	// 設定預設數量標籤
	QuantityTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Stack.Quantity"));
}

void UGZInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// 呼叫父類實作
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// 快取項目資料物件
	CachedListItemObject = Cast<UGZInventoryListItemObject>(ListItemObject);

	if (!IsValid(CachedListItemObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryEntryWidget::NativeOnListItemObjectSet - Invalid ListItemObject"));
		return;
	}

	// 驗證物品實例
	UGZInventoryItemInstance* ItemInstance = CachedListItemObject->GetItemInstance();
	if (!IsValid(ItemInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryEntryWidget::NativeOnListItemObjectSet - Invalid ItemInstance"));
		return;
	}

	// 更新顯示
	UpdateDisplay();

	// 觸發 BP 事件
	OnItemDataSet(CachedListItemObject);

	UE_LOG(LogTemp, VeryVerbose, TEXT("UGZInventoryEntryWidget::NativeOnListItemObjectSet - Set item: %s"),
	       *CachedListItemObject->GetItemName().ToString());
}

void UGZInventoryEntryWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	bIsCurrentlySelected = bIsSelected;

	// 觸發 BP 事件
	OnSelectionStateChanged(bIsSelected);

	UE_LOG(LogTemp, VeryVerbose, TEXT("UGZInventoryEntryWidget::NativeOnItemSelectionChanged - Selection: %s"),
	       bIsSelected ? TEXT("True") : TEXT("False"));
}

void UGZInventoryEntryWidget::NativeOnItemExpansionChanged(bool bIsExpanded)
{
	IUserObjectListEntry::NativeOnItemExpansionChanged(bIsExpanded);

	bIsCurrentlyExpanded = bIsExpanded;

	// 觸發 BP 事件
	OnExpansionStateChanged(bIsExpanded);

	UE_LOG(LogTemp, VeryVerbose, TEXT("UGZInventoryEntryWidget::NativeOnItemExpansionChanged - Expansion: %s"),
	       bIsExpanded ? TEXT("True") : TEXT("False"));
}

void UGZInventoryEntryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsDesignTime())
	{
		// 在設計時顯示預覽資料
		if (IsValid(ItemName))
		{
			ItemName->SetText(FText::FromString(TEXT("Preview Item")));
		}

		if (IsValid(ItemQuantity))
		{
			ItemQuantity->SetText(FText::FromString(TEXT("99")));
		}

		if (IsValid(ItemIcon) && IsValid(DefaultItemIcon))
		{
			ItemIcon->SetBrushFromTexture(DefaultItemIcon);
		}
	}
}

void UGZInventoryEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 初始化時更新顯示
	if (IsValid(CachedListItemObject))
	{
		UpdateDisplay();
	}
}

UGZInventoryItemInstance* UGZInventoryEntryWidget::GetItemInstance() const
{
	return IsValid(CachedListItemObject) ? CachedListItemObject->GetItemInstance() : nullptr;
}

UGZInventoryItemDefinition* UGZInventoryEntryWidget::GetItemDefinition() const
{
	return IsValid(CachedListItemObject) ? CachedListItemObject->GetItemDefinition() : nullptr;
}

void UGZInventoryEntryWidget::UpdateDisplay()
{
	if (!IsValid(CachedListItemObject))
	{
		return;
	}

	// 更新各個顯示元件
	UpdateItemIcon();
	UpdateItemName();
	UpdateItemQuantity();
}

void UGZInventoryEntryWidget::UpdateItemIcon()
{
	if (!IsValid(ItemIcon) || !IsValid(CachedListItemObject))
	{
		return;
	}

	UTexture2D* IconTexture = CachedListItemObject->GetItemIcon();
	if (!IsValid(IconTexture))
	{
		// 使用預設圖標
		IconTexture = DefaultItemIcon;
	}

	if (IsValid(IconTexture))
	{
		ItemIcon->SetBrushFromTexture(IconTexture);
	}
	else
	{
		// 清空圖標
		ItemIcon->SetBrushFromTexture(nullptr);
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryEntryWidget::UpdateItemIcon - No icon available for item: %s"),
		       *CachedListItemObject->GetItemName().ToString());
	}
}

void UGZInventoryEntryWidget::UpdateItemName()
{
	if (!IsValid(ItemName) || !IsValid(CachedListItemObject))
	{
		return;
	}

	const FName ItemNameValue = CachedListItemObject->GetItemName();
	if (ItemNameValue.IsNone())
	{
		ItemName->SetText(FText::FromString(TEXT("Unknown Item")));
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryEntryWidget::UpdateItemName - Item has no name"));
	}
	else
	{
		// 將 FName 轉換為本地化文本
		// 您可能需要根據實際的本地化系統調整這個邏輯
		FText DisplayName = FText::FromName(ItemNameValue);
		ItemName->SetText(DisplayName);
	}
}

void UGZInventoryEntryWidget::UpdateItemQuantity()
{
	if (!IsValid(ItemQuantity) || !IsValid(CachedListItemObject))
	{
		return;
	}

	const int32 Quantity = CachedListItemObject->GetItemQuantity(QuantityTag);

	// 根據設定決定是否顯示數量
	if (Quantity <= 1 && !bShowQuantityWhenOne)
	{
		// 隱藏數量文字
		ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		// 顯示數量
		ItemQuantity->SetVisibility(ESlateVisibility::Visible);
		ItemQuantity->SetText(FText::AsNumber(Quantity));
	}
}

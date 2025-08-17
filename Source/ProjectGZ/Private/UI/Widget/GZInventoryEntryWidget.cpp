#include "UI/Widget/GZInventoryEntryWidget.h"
#include "UI/Widget/GZInventoryWidget.h"
#include "Inventory/GZInventoryItemInstance.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Game/GZAssetManager.h"
#include "Game/GZGameplayTags.h"

UGZInventoryEntryWidget::UGZInventoryEntryWidget()
{
	// 設定預設數量標籤
	QuantityTag = GZGameplayTags::Item_Stack_Quantity;
}

void UGZInventoryEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// 呼叫父類實作
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// 快取項目資料物件
	CachedItemInstance = Cast<UGZInventoryItemInstance>(ListItemObject);

	if (!IsValid(CachedItemInstance))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryEntryWidget::NativeOnListItemObjectSet - Invalid ListItemObject"));
		return;
	}
	CachedItemDefinition = CachedItemInstance->GetItemDefinition();
	if (!IsValid(CachedItemDefinition))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryEntryWidget::NativeOnListItemObjectSet - Invalid ItemDefinition"));
		return;
	}
	// 更新顯示
	UpdateDisplay();

	// 觸發 BP 事件
	OnItemDataSet(CachedItemInstance);

	UE_LOG(LogTemp, VeryVerbose, TEXT("UGZInventoryEntryWidget::NativeOnListItemObjectSet - Set item: %s"),
	       *CachedItemInstance->GetItemDefinition()->GetItemName().ToString());
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
	if (IsValid(CachedItemInstance))
	{
		UpdateDisplay();
	}
}

UGZInventoryItemInstance* UGZInventoryEntryWidget::GetItemInstance() const
{
	return IsValid(CachedItemInstance) ? CachedItemInstance : nullptr;
}

UGZInventoryItemDefinition* UGZInventoryEntryWidget::GetItemDefinition() const
{
	return IsValid(CachedItemInstance) ? CachedItemInstance->GetItemDefinition() : nullptr;
}

void UGZInventoryEntryWidget::UpdateDisplay()
{
	if (!IsValid(CachedItemInstance) || !IsValid(CachedItemDefinition))
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
	if (!IsValid(ItemIcon) || !IsValid(CachedItemDefinition))
	{
		return;
	}

	UTexture2D* IconTexture = CachedItemDefinition->GetItemIcon();
	if (IconTexture)
	{
		ItemIcon->SetBrushFromTexture(IconTexture);
	}
	else
	{
		// 使用預設圖標
		ItemIcon->SetBrushFromTexture(DefaultItemIcon);
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "UGZInventoryEntryWidget::UpdateItemIcon - No icon available for item: %s"
		       ),
		       *CachedItemDefinition->GetItemName().ToString());
	}
}

void UGZInventoryEntryWidget::UpdateItemName()
{
	if (!IsValid(ItemName) || !IsValid(CachedItemDefinition))
	{
		return;
	}

	const FName ItemNameValue = CachedItemDefinition->GetItemName();
	if (ItemNameValue.IsNone())
	{
		ItemName->SetText(FText::FromString(TEXT("Unknown Item")));
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryEntryWidget::UpdateItemName - Item has no name"));
	}
	else
	{
		FText DisplayName = FText::FromName(ItemNameValue);
		ItemName->SetText(DisplayName);
	}
}

void UGZInventoryEntryWidget::UpdateItemQuantity()
{
	if (!IsValid(ItemQuantity) || !IsValid(CachedItemInstance))
	{
		return;
	}

	const int32 Quantity = CachedItemInstance->GetStackByTag(QuantityTag);

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

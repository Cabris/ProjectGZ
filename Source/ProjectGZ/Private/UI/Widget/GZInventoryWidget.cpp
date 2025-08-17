#include "UI/Widget/GZInventoryWidget.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Components/ListView.h"
#include "UI/WidgetController/GZInventoryWidgetController.h"

UGZInventoryWidget::UGZInventoryWidget()
{
	// 設定預設數量標籤 - 您可能需要根據實際的標籤系統調整
	DefaultQuantityTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Stack.Quantity"));
}

void UGZInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 嘗試綁定到控制器
	BindToController();
	// 觸發 BP 初始化事件
	OnInventoryWidgetInitialized();
}

void UGZInventoryWidget::NativeDestruct()
{
	// 清理資源
	UnbindFromController();
	RequestClearAllItems();

	Super::NativeDestruct();
}

void UGZInventoryWidget::SetListView(UListView* InListView)
{
	if (IsValid(InListView) && ListView != InListView)
	{
		ListView = InListView;
	}
}

void UGZInventoryWidget::RequestRefreshInventory()
{
}

void UGZInventoryWidget::RequestClearAllItems()
{
}

void UGZInventoryWidget::BindToController()
{
	if (bIsBoundToController)
	{
		return;
	}

	UGZInventoryWidgetController* InventoryController = GetWidgetController<UGZInventoryWidgetController>();
	if (!IsValid(InventoryController))
	{
		// Controller 可能還沒設定，稍後會再次嘗試綁定
		return;
	}

	// 綁定事件
	InventoryController->OnItemAdded.BindUObject(this, &ThisClass::HandleItemAdded);
	InventoryController->OnItemWillRemove.BindUObject(this, &ThisClass::HandleItemWillRemove);
	InventoryController->OnItemChanged.BindUObject(this, &ThisClass::HandleItemChanged);
	InventoryController->OnItemListInitialized.BindUObject(this, &ThisClass::HandleItemListInitialized);
	bIsBoundToController = true;

	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::BindToController - Successfully bound to controller"));
}

void UGZInventoryWidget::UnbindFromController()
{
	if (!bIsBoundToController)
	{
		return;
	}

	UGZInventoryWidgetController* InventoryController = GetWidgetController<UGZInventoryWidgetController>();
	if (IsValid(InventoryController))
	{
		InventoryController->OnItemAdded.Unbind();
		InventoryController->OnItemWillRemove.Unbind();
		InventoryController->OnItemChanged.Unbind();
		InventoryController->OnItemChanged.Unbind();
	}

	bIsBoundToController = false;
}

void UGZInventoryWidget::HandleItemListInitialized(const TArray<UGZInventoryItemInstance*>& ItemList)
{
	ListView->SetListItems(ItemList);
	// 觸發 BP 事件
	OnInventoryListCompletelyChanged();
	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::RebuildCompleteItemList - Rebuilt list with %d items"),
	       ItemList.Num());
}

void UGZInventoryWidget::HandleItemAdded(UGZInventoryItemInstance* ItemInstance)
{
	ListView->AddItem(ItemInstance);
	// 觸發 BP 事件
	OnInventoryItemAdded(ItemInstance);
	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::HandleItemsAdded - Added %d items"), 1);
}

void UGZInventoryWidget::HandleItemWillRemove(UGZInventoryItemInstance* ItemInstance)
{
	ListView->RemoveItem(ItemInstance);
	OnInventoryItemWillRemove(ItemInstance);
	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::HandleItemsRemoved - Removed %d items"), 1);
}

void UGZInventoryWidget::HandleItemChanged(UGZInventoryItemInstance* ItemInstance)
{
	auto EntryWidge = ListView->GetEntryWidgetFromItem(ItemInstance);
	if (!EntryWidge)return;
	UGZInventoryEntryWidget* InventoryEntryWidget = Cast<UGZInventoryEntryWidget>(EntryWidge);
	if (InventoryEntryWidget)
	{
		InventoryEntryWidget->UpdateDisplay();
	}
}

/*void UGZInventoryWidget::SortItemList(TArray<UGZInventoryListItemObject*>& ItemList)
{
	// 根據物品名稱排序 (可以擴展為更複雜的排序邏輯)
	ItemList.Sort([](const UGZInventoryListItemObject& A, const UGZInventoryListItemObject& B)
	{
		const FName NameA = A.GetItemName();
		const FName NameB = B.GetItemName();

		return NameA.LexicalLess(NameB);
	});
}*/

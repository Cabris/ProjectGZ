#include "UI/Widget/GZInventoryWidget.h"

#include "HeadMountedDisplayTypes.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Components/ListView.h"
#include "Game/GZGameplayTags.h"
#include "ProjectGZ/ProjectGZ.h"
#include "UI/WidgetController/GZInventoryWidgetController.h"

UGZInventoryWidget::UGZInventoryWidget()
{
	// 設定預設數量標籤 - 您可能需要根據實際的標籤系統調整
	DefaultQuantityTag = GZGameplayTags::Item_Stack_Quantity;
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

void UGZInventoryWidget::RequestRefreshInventory()
{
	UGZInventoryWidgetController* InventoryController = GetWidgetController<UGZInventoryWidgetController>();
	check(IsValid(InventoryController));
	InventoryController->RefreshInventory();
}

void UGZInventoryWidget::RequestClearAllItems()
{
	UGZInventoryWidgetController* InventoryController = GetWidgetController<UGZInventoryWidgetController>();
	check(IsValid(InventoryController));
	InventoryController->ClearAllItems();
}

void UGZInventoryWidget::BindToController()
{
	if (bIsBoundToController)
	{
		return;
	}

	UGZInventoryWidgetController* InventoryController = GetWidgetController<UGZInventoryWidgetController>();
	check(IsValid(InventoryController));
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
	check(IsValid(InventoryController));
	InventoryController->OnItemAdded.Unbind();
	InventoryController->OnItemWillRemove.Unbind();
	InventoryController->OnItemChanged.Unbind();
	InventoryController->OnItemChanged.Unbind();
	bIsBoundToController = false;
}

void UGZInventoryWidget::HandleItemListInitialized(const TArray<UGZInventoryItemInstance*>& ItemList)
{
	check(IsValid(ListView));
	ListView->SetListItems(ItemList);
	// 觸發 BP 事件
	OnInventoryListCompletelyChanged();
	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::RebuildCompleteItemList - Rebuilt list with %d items"),
	       ItemList.Num());
}

void UGZInventoryWidget::HandleItemAdded(UGZInventoryItemInstance* ItemInstance)
{
	check(IsValid(ListView));
	ListView->AddItem(ItemInstance);
	// 觸發 BP 事件
	OnInventoryItemAdded(ItemInstance);
	int32 Stack = ItemInstance->GetDefaultStack();
	Stack = 1;
	Debug::Print(FString::Printf(TEXT("UGZInventoryWidget::HandleItemsAdded - Added %d items"), Stack));
	//UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::HandleItemsAdded - Added %d items"), 1);
}

void UGZInventoryWidget::HandleItemWillRemove(UGZInventoryItemInstance* ItemInstance)
{
	check(IsValid(ListView));
	ListView->RemoveItem(ItemInstance);
	OnInventoryItemWillRemove(ItemInstance);
	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::HandleItemsRemoved - Removed %d items"), 1);
}

void UGZInventoryWidget::HandleItemChanged(UGZInventoryItemInstance* ItemInstance)
{
	check(IsValid(ListView));
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

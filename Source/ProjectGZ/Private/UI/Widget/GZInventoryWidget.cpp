#include "UI/Widget/GZInventoryWidget.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "Components/ListView.h"
#include "UI/Widget/Inventory/GZInventoryListItemObject.h"
#include "UI/WidgetController/GZInventoryWidgetController.h"

UGZInventoryWidget::UGZInventoryWidget()
{
	// 設定預設數量標籤 - 您可能需要根據實際的標籤系統調整
	DefaultQuantityTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Stack.Quantity"));
}

void UGZInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 初始化物件池
	InitializeObjectPool();
}

void UGZInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 確保物件池已初始化
	if (!bPoolInitialized)
	{
		InitializeObjectPool();
	}
	// 嘗試綁定到控制器
	BindToController();
	// 觸發 BP 初始化事件
	OnInventoryWidgetInitialized();
}

void UGZInventoryWidget::NativeDestruct()
{
	// 清理資源
	UnbindFromController();
	ClearAllItems();

	Super::NativeDestruct();
}

void UGZInventoryWidget::InitializeObjectPool()
{
	if (bPoolInitialized)
	{
		return;
	}

	// 創建物件池實例
	ObjectPool = NewObject<UGZObjectPool>(this);

	if (IsValid(ObjectPool))
	{
		// 初始化物件池
		ObjectPool->InitializePool(UGZInventoryListItemObject::StaticClass(), PoolConfig, this);

		// 綁定事件 (可選)
		ObjectPool->OnObjectCreated.AddDynamic(this, &ThisClass::OnPoolObjectCreated);
		ObjectPool->OnObjectReturned.AddDynamic(this, &ThisClass::OnPoolObjectReturned);

		bPoolInitialized = true;

		UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::InitializeObjectPool - Object pool initialized"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UGZInventoryWidget::InitializeObjectPool - Failed to create object pool"));
	}
}

UGZInventoryListItemObject* UGZInventoryWidget::GetFromPool()
{
	if (!IsValid(ObjectPool))
	{
		UE_LOG(LogTemp, Error, TEXT("UGZInventoryWidget::GetFromPool - Object pool is invalid"));
		return nullptr;
	}

	UObject* PooledObject = ObjectPool->GetFromPool();
	return Cast<UGZInventoryListItemObject>(PooledObject);
}

void UGZInventoryWidget::ReturnToPool(UGZInventoryListItemObject* Object)
{
	if (!IsValid(ObjectPool) || !IsValid(Object))
	{
		return;
	}
	Object->InitializeWithItemInstance(nullptr, -1);
	ObjectPool->ReturnToPool(Object);
}

void UGZInventoryWidget::SetListView(UListView* InListView)
{
	if (IsValid(InListView) && ListView != InListView)
	{
		ListView = InListView;
		// 如果有現有資料，重新整理顯示
		if (CurrentDisplayItems.Num() > 0)
		{
			RefreshInventoryDisplay();
		}
	}
}

void UGZInventoryWidget::RefreshInventoryDisplay()
{
	if (!IsValid(ListView))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryWidget::RefreshInventoryDisplay - No valid view widget found"));
		return;
	}

	// 清空當前顯示
	ListView->ClearListItems();

	// 重新添加項目
	for (UGZInventoryListItemObject* Item : CurrentDisplayItems)
	{
		if (IsValid(Item))
		{
			ListView->AddItem(Item);
		}
	}
}

void UGZInventoryWidget::ClearAllItems()
{
	// 清空 UI 顯示
	if (IsValid(ListView))
	{
		ListView->ClearListItems();
	}

	// 回收物件到池中
	for (UGZInventoryListItemObject* Item : CurrentDisplayItems)
	{
		if (IsValid(Item))
		{
			ReturnToPool(Item);
		}
	}

	CurrentDisplayItems.Empty();
}

int32 UGZInventoryWidget::GetDisplayedItemCount() const
{
	return CurrentDisplayItems.Num();
}

void UGZInventoryWidget::SetPoolConfig(const FGZObjectPoolConfig& NewConfig)
{
	PoolConfig = NewConfig;

	// 如果物件池已初始化，重新初始化
	if (bPoolInitialized && IsValid(ObjectPool))
	{
		ObjectPool->ClearPool();
		ObjectPool->InitializePool(UGZInventoryListItemObject::StaticClass(), PoolConfig, this);
	}
}

void UGZInventoryWidget::GetPoolStats(int32& OutAvailable, int32& OutTotal, int32& OutActive) const
{
	if (IsValid(ObjectPool))
	{
		ObjectPool->GetPoolStats(OutAvailable, OutTotal, OutActive);
	}
	else
	{
		OutAvailable = OutTotal = OutActive = 0;
	}
}

void UGZInventoryWidget::WarmUpPool(int32 Count)
{
	if (IsValid(ObjectPool))
	{
		ObjectPool->WarmUp(Count);
	}
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
	InventoryController->OnInventoryListUpdated.AddDynamic(this, &ThisClass::OnInventoryListUpdated);

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
		InventoryController->OnInventoryListUpdated.RemoveDynamic(this, &ThisClass::OnInventoryListUpdated);
	}

	bIsBoundToController = false;
}

void UGZInventoryWidget::OnInventoryListUpdated(const FInventoryListModifyData& ModifyData)
{
	switch (ModifyData.ModifyAction)
	{
	case EInventoryListModifyAction::ItemAdded:
		HandleItemsAdded(ModifyData);
		break;

	case EInventoryListModifyAction::ItemRemoved:
		HandleItemsRemoved(ModifyData);
		break;

	case EInventoryListModifyAction::ListChanged:
		RebuildCompleteItemList(ModifyData);
		break;

	default:
		UE_LOG(LogTemp, Warning, TEXT("UGZInventoryWidget::OnInventoryListChanged - Unknown modify action: %d"),
		       static_cast<int32>(ModifyData.ModifyAction));
		break;
	}
}

void UGZInventoryWidget::RebuildCompleteItemList(const FInventoryListModifyData& ModifyData)
{
	// 清空現有項目
	ClearAllItems();

	// 重建項目清單
	TArray<UGZInventoryListItemObject*> NewItems;
	NewItems.Reserve(ModifyData.ItemInstances.Num());

	for (const auto& ItemPair : ModifyData.ItemInstances)
	{
		const int32 ArrayIndex = ItemPair.Key;
		const FGZInventoryEntry& Entry = ItemPair.Value;
		if (IsValid(Entry.GetItemInstance()))
		{
			UGZInventoryListItemObject* ListItemObject = CreateListItemObject(Entry, ArrayIndex);
			if (IsValid(ListItemObject))
			{
				NewItems.Add(ListItemObject);
			}
		}
	}

	// 排序 (如果啟用)
	if (bEnableAutoSort)
	{
		SortItemList(NewItems);
	}

	CurrentDisplayItems = MoveTemp(NewItems);

	// 更新 UI 顯示
	RefreshInventoryDisplay();

	// 觸發 BP 事件
	OnInventoryListCompletelyChanged();

	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::RebuildCompleteItemList - Rebuilt list with %d items"),
	       CurrentDisplayItems.Num());
}

void UGZInventoryWidget::HandleItemsAdded(const FInventoryListModifyData& ModifyData)
{
	TArray<UGZInventoryListItemObject*> AddedItems;
	AddedItems.Reserve(ModifyData.ItemInstances.Num());

	for (const auto& ItemPair : ModifyData.ItemInstances)
	{
		const int32 ArrayIndex = ItemPair.Key;
		const FGZInventoryEntry& Entry = ItemPair.Value;

		if (IsValid(Entry.GetItemInstance()))
		{
			UGZInventoryListItemObject* ListItemObject = CreateListItemObject(Entry, ArrayIndex);
			if (IsValid(ListItemObject))
			{
				CurrentDisplayItems.Add(ListItemObject);
				AddedItems.Add(ListItemObject);

				// 立即添加到 UI
				ListView->AddItem(ListItemObject);
			}
		}
	}

	// 重新排序 (如果啟用)
	if (bEnableAutoSort && AddedItems.Num() > 0)
	{
		SortItemList(CurrentDisplayItems);
		RefreshInventoryDisplay();
	}

	// 觸發 BP 事件
	if (AddedItems.Num() > 0)
	{
		OnInventoryItemsAdded(AddedItems);
	}

	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::HandleItemsAdded - Added %d items"), AddedItems.Num());
}

void UGZInventoryWidget::HandleItemsRemoved(const FInventoryListModifyData& ModifyData)
{
	TArray<UGZInventoryListItemObject*> RemovedItems;

	// 根據陣列索引查找並移除項目
	for (const auto& ItemPair : ModifyData.ItemInstances)
	{
		const int32 ArrayIndex = ItemPair.Key;

		// 查找對應的 ListItemObject
		for (int32 i = CurrentDisplayItems.Num() - 1; i >= 0; --i)
		{
			UGZInventoryListItemObject* Item = CurrentDisplayItems[i];
			if (IsValid(Item) && Item->GetArrayIndex() == ArrayIndex)
			{
				RemovedItems.Add(Item);
				CurrentDisplayItems.RemoveAt(i);

				// 從 UI 中移除
				ListView->RemoveItem(Item);

				// 返回到物件池
				ReturnToPool(Item);
				break;
			}
		}
	}

	// 觸發 BP 事件
	if (RemovedItems.Num() > 0)
	{
		OnInventoryItemsRemoved(RemovedItems);
	}

	UE_LOG(LogTemp, Log, TEXT("UGZInventoryWidget::HandleItemsRemoved - Removed %d items"), RemovedItems.Num());
}

UGZInventoryListItemObject* UGZInventoryWidget::CreateListItemObject(const FGZInventoryEntry& Entry, int32 ArrayIndex)
{
	if (!IsValid(Entry.GetItemInstance()))
	{
		return nullptr;
	}

	// 從物件池獲取物件
	UGZInventoryListItemObject* ListItemObject = GetFromPool();

	if (IsValid(ListItemObject))
	{
		ListItemObject->InitializeWithItemInstance(Entry.GetItemInstance(), ArrayIndex);
	}

	return ListItemObject;
}

void UGZInventoryWidget::SortItemList(TArray<UGZInventoryListItemObject*>& ItemList)
{
	// 根據物品名稱排序 (可以擴展為更複雜的排序邏輯)
	ItemList.Sort([](const UGZInventoryListItemObject& A, const UGZInventoryListItemObject& B)
	{
		const FName NameA = A.GetItemName();
		const FName NameB = B.GetItemName();

		return NameA.LexicalLess(NameB);
	});
}

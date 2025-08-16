#include "Game/GZObjectPool.h"

#include "Interfactions/GZPoolableObject.h"

UGZObjectPool::UGZObjectPool() : bIsInitialized(false)
{
}

void UGZObjectPool::InitializePool(TSubclassOf<UObject> InObjectClass, const FGZObjectPoolConfig& Config, UObject* InOuter)
{
	if (bIsInitialized)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZObjectPool::InitializePool - Pool already initialized"));
		return;
	}

	if (!IsValid(InObjectClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UGZObjectPool::InitializePool - Invalid object class"));
		return;
	}

	FScopeLock Lock(&PoolMutex);

	ObjectClass = InObjectClass;
	PoolConfig = Config;
	Outer = InOuter ? InOuter : this;

	// 預分配陣列空間
	AvailableObjects.Reserve(PoolConfig.InitialPoolSize);
	AllObjects.Reserve(PoolConfig.MaxPoolSize > 0 ? PoolConfig.MaxPoolSize : PoolConfig.InitialPoolSize * 2);

	bIsInitialized = true;

	// 預熱池
	if (PoolConfig.InitialPoolSize > 0)
	{
		ExpandPool(PoolConfig.InitialPoolSize);
	}

	UE_LOG(LogTemp, Log, TEXT("UGZObjectPool::InitializePool - Initialized pool for class %s with %d objects"),
	       *ObjectClass->GetName(), PoolConfig.InitialPoolSize);
}

void UGZObjectPool::InitializePoolWithFactory(TSubclassOf<UObject> InObjectClass, const FGZObjectPoolConfig& Config,
                                              TFunction<UObject*()> InFactoryFunction, UObject* InOuter)
{
	FactoryFunction = MoveTemp(InFactoryFunction);
	InitializePool(InObjectClass, Config, InOuter);
}

UObject* UGZObjectPool::GetFromPool()
{
	if (!bIsInitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("UGZObjectPool::GetFromPool - Pool not initialized"));
		return nullptr;
	}

	FScopeLock Lock(&PoolMutex);

	UObject* Object = nullptr;

	// 從可用池中取得物件
	if (AvailableObjects.Num() > 0)
	{
		Object = AvailableObjects.Pop().Get();
	}
	// 池為空時的處理
	else if (PoolConfig.bAutoCreateWhenEmpty)
	{
		// 檢查是否達到最大池大小限制
		if (PoolConfig.MaxPoolSize > 0 && AllObjects.Num() >= PoolConfig.MaxPoolSize)
		{
			UE_LOG(LogTemp, Warning, TEXT("UGZObjectPool::GetFromPool - Pool reached maximum size limit"));
			return nullptr;
		}

		// 擴展池
		ExpandPool(PoolConfig.ExpansionStep);
		if (AvailableObjects.Num() > 0)
		{
			Object = AvailableObjects.Pop().Get();
		}
	}

	// 驗證物件有效性
	if (!IsValid(Object))
	{
		UE_LOG(LogTemp, Error, TEXT("UGZObjectPool::GetFromPool - Retrieved invalid object"));
		return nullptr;
	}

	// 調用物件的 OnGetFromPool 事件
	if (Object->GetClass()->ImplementsInterface(UGZPoolableObject::StaticClass()))
	{
		IGZPoolableObject::Execute_OnGetFromPool(Object);
	}

	return Object;
}

void UGZObjectPool::ReturnToPool(UObject* Object)
{
	if (!IsValid(Object))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZObjectPool::ReturnToPool - Attempting to return invalid object"));
		return;
	}

	if (!IsFromThisPool(Object))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZObjectPool::ReturnToPool - Object doesn't belong to this pool"));
		return;
	}

	FScopeLock Lock(&PoolMutex);

	// 檢查物件是否已經在可用池中
	if (AvailableObjects.Contains(Object))
	{
		UE_LOG(LogTemp, Warning, TEXT("UGZObjectPool::ReturnToPool - Object already in available pool"));
		return;
	}

	// 重置物件狀態
	if (PoolConfig.bResetOnReturn && Object->GetClass()->ImplementsInterface(UGZPoolableObject::StaticClass()))
	{
		IGZPoolableObject::Execute_ResetObjectState(Object);
	}

	// 調用物件的 OnReturnToPool 事件
	if (Object->GetClass()->ImplementsInterface(UGZPoolableObject::StaticClass()))
	{
		IGZPoolableObject::Execute_OnReturnToPool(Object);
	}

	// 檢查池大小限制
	if (PoolConfig.MaxPoolSize > 0 && AvailableObjects.Num() >= PoolConfig.MaxPoolSize)
	{
		// 池已滿，銷毀物件
		AllObjects.Remove(Object);
		Object->MarkAsGarbage();
		UE_LOG(LogTemp, Log, TEXT("UGZObjectPool::ReturnToPool - Pool full, destroying object"));
	}
	else
	{
		// 返回到可用池
		AvailableObjects.Add(Object);
		OnObjectReturned.Broadcast(Object);
	}
}

void UGZObjectPool::ClearPool()
{
	FScopeLock Lock(&PoolMutex);

	// 標記所有物件為垃圾回收
	for (TObjectPtr<UObject> Object : AllObjects)
	{
		if (IsValid(Object))
		{
			Object->MarkAsGarbage();
		}
	}

	AllObjects.Empty();
	AvailableObjects.Empty();

	UE_LOG(LogTemp, Log, TEXT("UGZObjectPool::ClearPool - Pool cleared"));
}

int32 UGZObjectPool::GetAvailableCount() const
{
	FScopeLock Lock(&PoolMutex);
	return AvailableObjects.Num();
}

int32 UGZObjectPool::GetTotalCount() const
{
	FScopeLock Lock(&PoolMutex);
	return AllObjects.Num();
}

void UGZObjectPool::WarmUp(int32 Count)
{
	if (!bIsInitialized || Count <= 0)
	{
		return;
	}

	FScopeLock Lock(&PoolMutex);
	ExpandPool(Count);

	UE_LOG(LogTemp, Log, TEXT("UGZObjectPool::WarmUp - Warmed up pool with %d objects"), Count);
}

void UGZObjectPool::ShrinkPool(int32 TargetSize)
{
	if (!bIsInitialized || TargetSize < 0)
	{
		return;
	}

	FScopeLock Lock(&PoolMutex);

	const int32 CurrentAvailable = AvailableObjects.Num();
	if (CurrentAvailable <= TargetSize)
	{
		return;
	}

	const int32 ObjectsToRemove = CurrentAvailable - TargetSize;
	for (int32 i = 0; i < ObjectsToRemove && AvailableObjects.Num() > 0; ++i)
	{
		TObjectPtr<UObject> Object = AvailableObjects.Pop();
		if (IsValid(Object))
		{
			AllObjects.Remove(Object);
			Object->MarkAsGarbage();
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UGZObjectPool::ShrinkPool - Shrunk pool by %d objects"), ObjectsToRemove);
}

bool UGZObjectPool::IsFromThisPool(UObject* Object) const
{
	if (!IsValid(Object))
	{
		return false;
	}

	FScopeLock Lock(&PoolMutex);
	return AllObjects.Contains(Object);
}

void UGZObjectPool::GetPoolStats(int32& OutAvailableCount, int32& OutTotalCount, int32& OutActiveCount) const
{
	FScopeLock Lock(&PoolMutex);

	OutAvailableCount = AvailableObjects.Num();
	OutTotalCount = AllObjects.Num();
	OutActiveCount = OutTotalCount - OutAvailableCount;
}

UObject* UGZObjectPool::CreateNewObject()
{
	if (!IsValid(ObjectClass))
	{
		return nullptr;
	}

	UObject* CreatedObject = nullptr;

	// 使用自定義工廠函數 (如果有)
	if (FactoryFunction)
	{
		CreatedObject = FactoryFunction();
	}
	else
	{
		// 使用標準 NewObject
		CreatedObject = NewObject<UObject>(Outer, ObjectClass);
	}

	if (IsValid(CreatedObject))
	{
		AllObjects.Add(CreatedObject);
		OnObjectCreated.Broadcast(CreatedObject);
	}

	return CreatedObject;
}

void UGZObjectPool::ExpandPool(int32 Count)
{
	if (Count <= 0)
	{
		return;
	}

	// 檢查最大池大小限制
	const int32 CurrentTotal = AllObjects.Num();
	const int32 MaxAllowed = PoolConfig.MaxPoolSize > 0 ? PoolConfig.MaxPoolSize : INT32_MAX;
	const int32 ActualCount = FMath::Min(Count, MaxAllowed - CurrentTotal);

	if (ActualCount <= 0)
	{
		return;
	}

	// 創建新物件
	for (int32 i = 0; i < ActualCount; ++i)
	{
		UObject* NewObject = CreateNewObject();
		if (IsValid(NewObject))
		{
			AvailableObjects.Add(NewObject);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UGZObjectPool::ExpandPool - Failed to create object %d"), i);
			break;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("UGZObjectPool::ExpandPool - Expanded pool by %d objects"), ActualCount);
}

bool UGZObjectPool::IsValidPoolObject(UObject* Object) const
{
	return IsValid(Object) && Object->IsA(ObjectClass);
}

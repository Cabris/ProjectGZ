#include "Game/GZObjectPoolManager.h"
#include "Game/GZObjectPool.h"

TObjectPtr<UGZObjectPoolManager> UGZObjectPoolManager::GlobalInstance = nullptr;

UGZObjectPoolManager* UGZObjectPoolManager::GetGlobalPoolManager()
{
	if (!IsValid(GlobalInstance))
	{
		GlobalInstance = NewObject<UGZObjectPoolManager>(GetTransientPackage());
		GlobalInstance->AddToRoot(); // 防止被 GC
	}
	return GlobalInstance;
}

UGZObjectPool* UGZObjectPoolManager::GetOrCreatePool(TSubclassOf<UObject> ObjectClass, const FGZObjectPoolConfig& Config)
{
	if (!IsValid(ObjectClass))
	{
		UE_LOG(LogTemp, Error, TEXT("UGZObjectPoolManager::GetOrCreatePool - Invalid object class"));
		return nullptr;
	}

	FScopeLock Lock(&ManagerMutex);

	// 檢查是否已存在池
	if (TObjectPtr<UGZObjectPool>* ExistingPool = ObjectPools.Find(ObjectClass))
	{
		if (IsValid(*ExistingPool))
		{
			return *ExistingPool;
		}
		else
		{
			// 移除無效的池
			ObjectPools.Remove(ObjectClass);
		}
	}

	// 創建新池
	UGZObjectPool* NewPool = NewObject<UGZObjectPool>(this);
	if (IsValid(NewPool))
	{
		NewPool->InitializePool(ObjectClass, Config, this);
		ObjectPools.Add(ObjectClass, NewPool);

		UE_LOG(LogTemp, Log, TEXT("UGZObjectPoolManager::GetOrCreatePool - Created new pool for class %s"), 
			*ObjectClass->GetName());
	}

	return NewPool;
}

UGZObjectPool* UGZObjectPoolManager::GetPool(TSubclassOf<UObject> ObjectClass) const
{
	if (!IsValid(ObjectClass))
	{
		return nullptr;
	}

	FScopeLock Lock(&ManagerMutex);

	if (const TObjectPtr<UGZObjectPool>* Pool = ObjectPools.Find(ObjectClass))
	{
		return IsValid(*Pool) ? *Pool : nullptr;
	}

	return nullptr;
}

void UGZObjectPoolManager::RemovePool(TSubclassOf<UObject> ObjectClass)
{
	if (!IsValid(ObjectClass))
	{
		return;
	}

	FScopeLock Lock(&ManagerMutex);

	if (TObjectPtr<UGZObjectPool>* Pool = ObjectPools.Find(ObjectClass))
	{
		if (IsValid(*Pool))
		{
			(*Pool)->ClearPool();
		}
		ObjectPools.Remove(ObjectClass);

		UE_LOG(LogTemp, Log, TEXT("UGZObjectPoolManager::RemovePool - Removed pool for class %s"), 
			*ObjectClass->GetName());
	}
}

void UGZObjectPoolManager::ClearAllPools()
{
	FScopeLock Lock(&ManagerMutex);

	for (auto& Pair : ObjectPools)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->ClearPool();
		}
	}

	ObjectPools.Empty();

	UE_LOG(LogTemp, Log, TEXT("UGZObjectPoolManager::ClearAllPools - Cleared all pools"));
}

void UGZObjectPoolManager::GetAllPoolsStats(TArray<TSubclassOf<UObject>>& OutClasses, TArray<int32>& OutAvailableCounts, 
	TArray<int32>& OutTotalCounts) const
{
	FScopeLock Lock(&ManagerMutex);

	OutClasses.Empty();
	OutAvailableCounts.Empty();
	OutTotalCounts.Empty();

	for (const auto& Pair : ObjectPools)
	{
		if (IsValid(Pair.Value))
		{
			OutClasses.Add(Pair.Key);
			OutAvailableCounts.Add(Pair.Value->GetAvailableCount());
			OutTotalCounts.Add(Pair.Value->GetTotalCount());
		}
	}
}

void UGZObjectPoolManager::BeginDestroy()
{
	ClearAllPools();

	if (GlobalInstance == this)
	{
		GlobalInstance = nullptr;
	}

	Super::BeginDestroy();
}
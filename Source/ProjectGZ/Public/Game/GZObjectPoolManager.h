#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GZObjectPoolManager.generated.h"

struct FGZObjectPoolConfig;
class UGZObjectPool;

UCLASS()
class PROJECTGZ_API UGZObjectPoolManager : public UObject
{
	GENERATED_BODY()

public:
	/** 獲取全域物件池管理器實例 */
	UFUNCTION(BlueprintPure, Category = "Object Pool", CallInEditor)
	static UGZObjectPoolManager* Get();

	/** 創建或獲取指定類型的物件池 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	UGZObjectPool* GetOrCreatePool(TSubclassOf<UObject> ObjectClass, const FGZObjectPoolConfig& Config);

	/** 獲取現有的物件池 */
	UFUNCTION(BlueprintPure, Category = "Object Pool")
	UGZObjectPool* GetPool(TSubclassOf<UObject> ObjectClass) const;

	/** 移除物件池 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void RemovePool(TSubclassOf<UObject> ObjectClass);

	/** 清空所有池 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ClearAllPools();

	/** 獲取所有池的統計資訊 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void GetAllPoolsStats(TArray<TSubclassOf<UObject>>& OutClasses, TArray<int32>& OutAvailableCounts,
	                      TArray<int32>& OutTotalCounts) const;

protected:
	virtual void BeginDestroy() override;

private:
	/** 物件池映射 */
	UPROPERTY(Transient)
	TMap<TSubclassOf<UObject>, TObjectPtr<UGZObjectPool>> ObjectPools;

	/** 執行序安全鎖 */
	mutable FCriticalSection ManagerMutex;

	/** 全域實例 */
	static TObjectPtr<UGZObjectPoolManager> GlobalInstance;
};

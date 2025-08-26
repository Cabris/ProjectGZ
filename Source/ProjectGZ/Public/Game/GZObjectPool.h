#pragma once

#include "CoreMinimal.h"
#include "Interfactions/GZObjectPoolInterface.h"
#include "UObject/Object.h"
#include "GZObjectPool.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectCreated, UObject*, CreatedObject);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectReturned, UObject*, ReturnedObject);

/**
 * 物件池配置結構
 */
USTRUCT(BlueprintType)
struct PROJECTGZ_API FGZObjectPoolConfig
{
	GENERATED_BODY()

	/** 初始池大小 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Config", meta = (ClampMin = "0"))
	int32 InitialPoolSize = 10;

	/** 最大池大小 (0 = 無限制) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Config", meta = (ClampMin = "0"))
	int32 MaxPoolSize = 100;

	/** 是否在池為空時自動創建新物件 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Config")
	bool bAutoCreateWhenEmpty = true;

	/** 是否在返回池時重置物件 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Config")
	bool bResetOnReturn = true;

	/** 池擴展步長 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool Config", meta = (ClampMin = "1"))
	int32 ExpansionStep = 5;

	FGZObjectPoolConfig()
		: InitialPoolSize(10)
		  , MaxPoolSize(100)
		  , bAutoCreateWhenEmpty(true)
		  , bResetOnReturn(true)
		  , ExpansionStep(5)
	{
	}
};

UCLASS()
class PROJECTGZ_API UGZObjectPool : public UObject, public IGZObjectPoolInterface
{
	GENERATED_BODY()

public:
	UGZObjectPool();

	/** 初始化物件池 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void InitializePool(TSubclassOf<UObject> InObjectClass, const FGZObjectPoolConfig& Config,
	                    UObject* InOuter = nullptr);

	/** 初始化物件池 (C++ 版本，支援自定義工廠函數) */
	void InitializePoolWithFactory(TSubclassOf<UObject> InObjectClass, const FGZObjectPoolConfig& Config,
	                               TFunction<UObject*()> InFactoryFunction, UObject* InOuter = nullptr);

	// IGZObjectPoolInterface 實現
	virtual UObject* GetFromPool() override;
	virtual void ReturnToPool(UObject* Object) override;
	virtual void ClearPool() override;
	virtual int32 GetAvailableCount() const override;
	virtual int32 GetTotalCount() const override;

	/** 預熱池 - 創建指定數量的物件 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void WarmUp(int32 Count);

	/** 收縮池 - 移除多餘的可用物件 */
	UFUNCTION(BlueprintCallable, Category = "Object Pool")
	void ShrinkPool(int32 TargetSize);

	/** 檢查物件是否屬於此池 */
	UFUNCTION(BlueprintPure, Category = "Object Pool")
	bool IsFromThisPool(UObject* Object) const;

	/** 獲取池統計資訊 */
	UFUNCTION(BlueprintPure, Category = "Object Pool")
	void GetPoolStats(int32& OutAvailableCount, int32& OutTotalCount, int32& OutActiveCount) const;

	/** 事件委託 */
	UPROPERTY(BlueprintAssignable, Category = "Object Pool")
	FOnObjectCreated OnObjectCreated;

	UPROPERTY(BlueprintAssignable, Category = "Object Pool")
	FOnObjectReturned OnObjectReturned;

	template <typename T>
	T* GetFromPool()
	{
		UObject* Obj = GetFromPool();
		if (!Obj) return nullptr;
		return Cast<T>(Obj);
	}

protected:
	/** 創建新物件 */
	UObject* CreateNewObject();

	/** 擴展池大小 */
	void ExpandPool(int32 Count);

	/** 驗證物件有效性 */
	bool IsValidPoolObject(UObject* Object) const;

private:
	/** 物件類型 */
	UPROPERTY(Transient)
	TSubclassOf<UObject> ObjectClass;

	/** 物件外部容器 */
	UPROPERTY(Transient)
	TObjectPtr<UObject> Outer;

	/** 池配置 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Object Pool", meta = (AllowPrivateAccess = "true"))
	FGZObjectPoolConfig PoolConfig;

	/** 可用物件池 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UObject>> AvailableObjects;

	/** 所有創建的物件 (用於追蹤和清理) */
	UPROPERTY(Transient)
	TSet<TObjectPtr<UObject>> AllObjects;

	/** 執行序安全鎖 */
	mutable FCriticalSection PoolMutex;

	/** 自定義工廠函數 (C++ 使用) */
	TFunction<UObject*()> FactoryFunction;

	/** 是否已初始化 */
	bool bIsInitialized = false;
};

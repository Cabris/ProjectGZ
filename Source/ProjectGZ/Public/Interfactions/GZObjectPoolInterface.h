#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GZObjectPoolInterface.generated.h"

UINTERFACE()
class UGZObjectPoolInterface : public UInterface
{
	GENERATED_BODY()
};
class PROJECTGZ_API IGZObjectPoolInterface
{
	GENERATED_BODY()
public:
	/** 從池中獲取物件 */
	virtual UObject* GetFromPool() = 0;
	
	/** 將物件返回到池中 */
	virtual void ReturnToPool(UObject* Object) = 0;
	
	/** 清空池 */
	virtual void ClearPool() = 0;
	
	/** 獲取池中可用物件數量 */
	virtual int32 GetAvailableCount() const = 0;
	
	/** 獲取池中總物件數量 */
	virtual int32 GetTotalCount() const = 0;
};

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GZPoolableObject.generated.h"

UINTERFACE()
class UGZPoolableObject : public UInterface
{
	GENERATED_BODY()
};

class PROJECTGZ_API IGZPoolableObject
{
	GENERATED_BODY()

public:
	/** 物件被從池中取出時調用 */
	UFUNCTION(BlueprintNativeEvent, Category = "Object Pool")
	void OnGetFromPool();

	/** 物件被返回池中時調用 */
	UFUNCTION(BlueprintNativeEvent, Category = "Object Pool")
	void OnReturnToPool();

	/** 重置物件狀態 */
	UFUNCTION(BlueprintNativeEvent, Category = "Object Pool")
	void ResetObjectState();
};

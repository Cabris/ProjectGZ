#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inventory/GZInventoryItemDefinition.h"
#include "GZCollectable.generated.h"

UINTERFACE()
class UGZCollectable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTGZ_API IGZCollectable
{
	GENERATED_BODY()

public:
	virtual void ConsumeItemQuantity(int32 Quantity) =0;
	virtual const TSubclassOf<UGZInventoryItemDefinition>& GetItemDefinitionClass() const =0;
	UFUNCTION(BlueprintNativeEvent, Category="Collectable")
	void OnCollected();
};

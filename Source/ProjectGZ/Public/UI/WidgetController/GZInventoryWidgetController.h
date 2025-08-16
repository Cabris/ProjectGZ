#pragma once

#include "CoreMinimal.h"
#include "GZWidgetController.h"
#include "Inventory/GZInventoryManagerComponent.h"
#include "GZInventoryWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryListModifySingnature, const FInventoryListModifyData&, ModifyData);

UCLASS()
class PROJECTGZ_API UGZInventoryWidgetController : public UGZWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
	UPROPERTY(BlueprintAssignable)//some or all item changed
	FInventoryListModifySingnature OnInventoryListUpdated;

protected:
	void OnInventoryModified(FGameplayTag Channel, const FInventoryListModifyData& ModifyData);
};

#pragma once

#include "CoreMinimal.h"
#include "GZWidgetController.h"
#include "GZEquipmentWidgetController.generated.h"

UCLASS()
class PROJECTGZ_API UGZEquipmentWidgetController : public UGZWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
private:
private:
};

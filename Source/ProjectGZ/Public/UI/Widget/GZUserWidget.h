#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/WidgetController/GZWidgetController.h"
#include "GZUserWidget.generated.h"

class UGZWidgetController;

UCLASS()
class PROJECTGZ_API UGZUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UGZWidgetController* Controller);

protected:
	UPROPERTY(BlueprintReadOnly, Category="Widget")
	TObjectPtr<UGZWidgetController> WidgetController;
	UFUNCTION(BlueprintImplementableEvent, Category="Widget")
	void WidgetControllerSet();

	template <typename WidgetControllerType>
	WidgetControllerType* GetWidgetController()
	{
		if (!IsValid(WidgetController))
			return nullptr;
		return Cast<WidgetControllerType>(WidgetController);
	}

private:
};

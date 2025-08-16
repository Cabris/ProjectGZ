#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GZHUD.generated.h"

class UGZInventoryWidgetController;
class UGZInventoryWidget;
class UGZOverlayWidgetController;
class UGZUserWidget;
class UGZWidgetController;
class UGZAbilitySystemComponent;
class AGZPlayerState;
class UGZAttributeSet;
class AGZPlayerController;

UCLASS()
class PROJECTGZ_API AGZHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitializeOverlay(AGZPlayerController* PC, AGZPlayerState* PS,
	                       UGZAbilitySystemComponent* ASC, UGZAttributeSet* AS);

protected:
	UPROPERTY(BlueprintReadOnly, Category="Overlay")
	TObjectPtr<UGZUserWidget> OverlayWidget = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Overlay")
	TObjectPtr<UGZOverlayWidgetController> OverlayWidgetController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Overlay")
	TSubclassOf<UGZUserWidget> OverlayWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category="Overlay")
	TSubclassOf<UGZOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UGZInventoryWidget> InventoryWidget = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TObjectPtr<UGZInventoryWidgetController> InventoryWidgetController = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UGZInventoryWidget> InventoryWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category="Inventory")
	TSubclassOf<UGZInventoryWidgetController> InventoryWidgetControllerClass;

private:
	template <typename WidgetType>
	TObjectPtr<WidgetType> GetOrCreateWidget(TObjectPtr<WidgetType> Widget, const TSubclassOf<WidgetType>& WidgetClass)
	{
		if (IsValid(Widget))
			return Widget;
		WidgetType* UserWidget = CreateWidget<WidgetType>(GetWorld(), WidgetClass);
		check(UserWidget);
		Widget = UserWidget;
		return Widget;
	}

	template <typename ControllerType>
	TObjectPtr<ControllerType> GetWidgetController(TObjectPtr<ControllerType>& Controller,
	                                               const TSubclassOf<ControllerType>& ControllerClass)
	{
		if (IsValid(Controller))
			return Controller;
		ControllerType* NewWidgetController = NewObject<ControllerType>(this, ControllerClass);
		check(NewWidgetController);
		Controller = NewWidgetController;
		return Controller;
	}
};

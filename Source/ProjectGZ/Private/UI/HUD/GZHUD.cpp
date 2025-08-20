#include "UI/HUD/GZHUD.h"

#include "UI/Widget/GZInventoryWidget.h"
#include "UI/Widget/GZUserWidget.h"
#include "UI/WidgetController/GZInventoryWidgetController.h"
#include "UI/WidgetController/GZOverlayWidgetController.h"
#include "UI/WidgetController/GZWidgetController.h"

void AGZHUD::InitializeOverlay(AGZPlayerController* PC, AGZPlayerState* PS, UGZAbilitySystemComponent* ASC, UGZAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass is not set!"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass is not set!"));
	FWidgetControllerParams WidgetParams(PC,PS);

	/*UGZUserWidget* UserWidget = CreateWidget<UGZUserWidget>(GetWorld(), OverlayWidgetClass);
	check(UserWidget);
	OverlayWidget = UserWidget;*/
	OverlayWidget = GetOrCreateWidget(OverlayWidget, OverlayWidgetClass);

	/*if (!IsValid(OverlayWidgetController))
	{
		UGZOverlayWidgetController* WidgetController = NewObject<UGZOverlayWidgetController>(this, OverlayWidgetControllerClass);
		check(WidgetController);
		OverlayWidgetController = WidgetController;
	}*/
	OverlayWidgetController = GetWidgetController(OverlayWidgetController, OverlayWidgetControllerClass);
	OverlayWidgetController->SetWidgetControllerParams(WidgetParams);
	OverlayWidget->SetWidgetController(OverlayWidgetController);
	OverlayWidgetController->BindCallbacksToDependencies();
	OverlayWidgetController->BroadcastInitialValues();
	OverlayWidget->AddToViewport();

	checkf(InventoryWidgetClass, TEXT("InventoryWidgetClass is not set!"));
	checkf(InventoryWidgetControllerClass, TEXT("InventoryWidgetControllerClass is not set!"));
	InventoryWidget = GetOrCreateWidget(InventoryWidget, InventoryWidgetClass);
	InventoryWidgetController = GetWidgetController(InventoryWidgetController, InventoryWidgetControllerClass);
	InventoryWidgetController->SetWidgetControllerParams(WidgetParams);
	InventoryWidget->SetWidgetController(InventoryWidgetController);
	InventoryWidgetController->BindCallbacksToDependencies();
	InventoryWidgetController->BroadcastInitialValues();
	InventoryWidget->AddToViewport();
}

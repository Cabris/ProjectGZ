#include "UI/HUD/GZHUD.h"
#include "UI/Widget/GZUserWidget.h"
#include "UI/WidgetController/GZWidgetController.h"

void AGZHUD::InitializeOverlay(AGZPlayerController* PC, AGZPlayerState* PS, UGZAbilitySystemComponent* ASC, UGZAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass is not set!"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass is not set!"));
	UGZUserWidget* UserWidget = CreateWidget<UGZUserWidget>(GetWorld(), OverlayWidgetClass);
	check(UserWidget);
	OverlayWidget = UserWidget;

	if (!IsValid(OverlayWidgetController))
	{
		UGZWidgetController* WidgetController = NewObject<UGZWidgetController>(this, OverlayWidgetControllerClass);
		check(WidgetController);
		OverlayWidgetController = WidgetController;
	}
	FWidgetControllerParams WidgetParams(PC, PS, ASC, AS);
	OverlayWidgetController->SetWidgetControllerParams(WidgetParams);
	OverlayWidgetController->BindCallbacksToDependencies();
	OverlayWidget->AddToViewport();
}

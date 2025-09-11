#include "UI/Widget/GZUserWidget.h"


void UGZUserWidget::SetWidgetController(UGZWidgetController* InController)
{
	WidgetController = InController;
	WidgetControllerSet();
}

void UGZUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGZUserWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

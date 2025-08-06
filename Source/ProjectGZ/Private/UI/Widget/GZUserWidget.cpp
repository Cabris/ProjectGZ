#include "UI/Widget/GZUserWidget.h"


void UGZUserWidget::SetWidgetController(UGZWidgetController* InController)
{
	WidgetController = InController;
	WidgetControllerSet();
}

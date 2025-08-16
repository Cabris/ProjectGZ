#include "UI/WidgetController/GZWidgetController.h"

void UGZWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
	PawnFeatureComponent = WidgetControllerParams.PawnFeature;
}

void UGZWidgetController::BroadcastInitialValues()
{
}

void UGZWidgetController::BindCallbacksToDependencies()
{
}

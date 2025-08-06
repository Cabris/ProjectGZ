// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/GZWidgetController.h"

void UGZWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributeSet = WidgetControllerParams.AttributeSet;
}

void UGZWidgetController::BroadcastInitialValues()
{
}

void UGZWidgetController::BindCallbacksToDependencies()
{
}

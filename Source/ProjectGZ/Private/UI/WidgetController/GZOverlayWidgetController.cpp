#include "UI/WidgetController/GZOverlayWidgetController.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZAttributeSet.h"

void UGZOverlayWidgetController::BroadcastInitialValues()
{
	if (!AttributeSet) return;
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
}

void UGZOverlayWidgetController::BindCallbacksToDependencies()
{
	if (!AbilitySystemComponent)return;
	auto& HealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGZAttributeSet::GetHealthAttribute());
	HealthChangeDelegate.AddUObject(this, &UGZOverlayWidgetController::HealthChanged);
	auto& MaxHealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGZAttributeSet::GetMaxHealthAttribute());
	MaxHealthChangeDelegate.AddUObject(this, &UGZOverlayWidgetController::MaxHealthChanged);
}

void UGZOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
}

void UGZOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnMaxHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
}

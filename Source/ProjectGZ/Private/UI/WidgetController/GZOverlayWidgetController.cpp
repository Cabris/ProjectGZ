#include "UI/WidgetController/GZOverlayWidgetController.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZAttributeSet.h"
#include "Game/GameplayEventMessage.h"
#include "Game/GZGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"

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

	if (EffectAppliedMessageListenerHandle.IsValid())
		EffectAppliedMessageListenerHandle.Unregister();
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	EffectAppliedMessageListenerHandle = MessageSystem.RegisterListener<FGZVerbMessage>(GZGameplayTags::MessageTag_Effect_Applied,
		[this](FGameplayTag Channel, const FGZVerbMessage& Payload)
		{
			auto& Tags = Payload.ContextTags;
			for (FGameplayTag Tag : Tags)
			{
				FUIWidgetDataRow* Row=GetDataTableRowByTag<FUIWidgetDataRow>(MessageWidgetDataTable,Tag);
				if (Row)
				{
					OnMessageWidgetRow.Broadcast(*Row);
				}
			}
		}
	);
}

void UGZOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
}

void UGZOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const
{
	OnMaxHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
}

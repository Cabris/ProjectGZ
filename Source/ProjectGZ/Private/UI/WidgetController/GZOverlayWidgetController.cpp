#include "UI/WidgetController/GZOverlayWidgetController.h"
#include "AbilitySystem/GZAbilitySystemComponent.h"
#include "AbilitySystem/GZAttributeSet.h"
#include "AssetRegistry/PackageReader.h"
#include "Character/GZPawnFeatureComponent.h"
#include "Game/GameplayEventMessage.h"
#include "Game/GZGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Player/GZPlayerController.h"

void UGZOverlayWidgetController::BroadcastInitialValues()
{
	if (!AttributeSet) return;
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
}

void UGZOverlayWidgetController::BindCallbacksToDependencies()
{
	if (AbilitySystemComponent)
	{
		auto& HealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UGZAttributeSet::GetHealthAttribute());
		HealthChangeDelegate.AddWeakLambda(this, [this](const FOnAttributeChangeData& OnAttributeChangeData)
		{
			OnHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
		});

		auto& MaxHealthChangeDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UGZAttributeSet::GetMaxHealthAttribute());
		MaxHealthChangeDelegate.AddWeakLambda(this, [this](const FOnAttributeChangeData& OnAttributeChangeData)
		{
			OnMaxHealthChanged.Broadcast(OnAttributeChangeData.NewValue);
		});
	}

	if (PawnFeatureComponent)
	{
		PawnFeatureComponent->OnFocusActor.AddWeakLambda(this, [this](AActor* FocusActor)
		{
			if (MarkerManager)
			{
				auto Marker = MarkerManager->AddMarkerTarget(FocusActor);
				OnMarkerWidgetAdded.Broadcast(Marker);//Widget add to canvas
			} //OnActorFocused.Broadcast(FocusActor);
		});
		PawnFeatureComponent->OnUnfocusActor.AddWeakLambda(this, [this](AActor* UnfocusActor)
		{
			if (MarkerManager)
			{
				auto Marker = MarkerManager->RemoveMarkerTarget(UnfocusActor);
				OnMarkerWidgetRemoved.Broadcast(Marker);//Widget remove from canvas
			} //OnActorUnfocused.Broadcast(UnfocusActor);
		});
	}

	if (EffectAppliedMessageListenerHandle.IsValid())
		EffectAppliedMessageListenerHandle.Unregister();
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	EffectAppliedMessageListenerHandle = MessageSystem.RegisterListener<FGZVerbMessage>(
		GZGameplayTags::MessageTag_Effect_Applied, this, &ThisClass::OnMessage);
}

void UGZOverlayWidgetController::OnWidgetControllerParamsSet()
{
	checkf(MarkerManagerClass, TEXT("UGZOverlayWidgetController::MarkerManagerClass is NUll"));
	checkf(PlayerController, TEXT("UGZOverlayWidgetController::PlayerController is NUll"));
	MarkerManager = NewObject<UGZMarkerManager>(this, MarkerManagerClass);
	MarkerManager->Initialize(MarkerConfig, PlayerController);
}

void UGZOverlayWidgetController::OnMessage(FGameplayTag Channel, const FGZVerbMessage& Payload)
{
	auto& Tags = Payload.ContextTags;
	for (FGameplayTag Tag : Tags)
	{
		FUIWidgetDataRow* Row = GetDataTableRowByTag<FUIWidgetDataRow>(MessageWidgetDataTable, Tag);
		if (Row)
		{
			OnMessageWidgetRow.Broadcast(*Row);
		}
	}
}

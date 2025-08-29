#include "UI/WidgetController/GZMarkerManager.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Game/GZObjectPoolManager.h"
#include "ProjectGZ/ProjectGZ.h"
#include "UI/Widget/GZMarkerWidget.h"

UGZMarkerManager::UGZMarkerManager()
{
	bIsInitialized = false;
	ObjectPool = nullptr;
	MarkerConfig = FGZMarkerConfig();
	MarkingRate = 30;
}

void UGZMarkerManager::Initialize(const FGZMarkerConfig& Config, APlayerController* PC)
{
	if (bIsInitialized)
	{
		Debug::Print(TEXT("UGZMarkerManager::Initialize: already Initialized"));
		return;
	}
	MarkerConfig = Config;
	check(PC);
	auto PoolManager = UGZObjectPoolManager::Get();
	check(PoolManager);
	check(MarkerConfig.MarkerWidgetClass);
	ObjectPool = PoolManager->GetOrCreatePool(MarkerConfig.MarkerWidgetClass, PoolConfig);
	check(ObjectPool);
	ActiveMarkers.Reserve(PoolConfig.InitialPoolSize);
	PlayerController = PC;
	StartMarking();
	bIsInitialized = true;
}

UGZMarkerWidget* UGZMarkerManager::AddMarkerTarget(AActor* Target)
{
	if (!bIsInitialized)
	{
		Debug::Print(TEXT("UGZMarkerManager::AddMarkerTarget: not Initialize"));
		return nullptr;
	}
	UGZMarkerWidget* MarkerWidget = ObjectPool->GetFromPool<UGZMarkerWidget>();
	MarkerWidget->SetTarget(Target, PlayerController);
	FSlotConfig Config;
	Config.SlotAlignment = FVector2D(0.5f, 0.5f);
	Config.SlotAnchors = FVector4f(0.0f, 0.0f, 0.0f, 0.0f);
	Config.SlotZOrder = MarkerConfig.ZOrder;
	MarkerWidget->SlotConfig = Config;
	ActiveMarkers.Add(Target, MarkerWidget);
	// UCanvasPanelSlot* Slot = MarkerCanvas->AddChildToCanvas(MarkerWidget);
	return MarkerWidget;
}

UGZMarkerWidget* UGZMarkerManager::RemoveMarkerTarget(AActor* Target)
{
	if (!bIsInitialized)
	{
		Debug::Print(TEXT("UGZMarkerManager::RemoveMarkerTarget: not Initialize"));
	}
	if (ActiveMarkers.Contains(Target))
	{
		auto MarkerWidget = ActiveMarkers[Target];
		//MarkerCanvas->RemoveChild(MarkerWidget);
		ActiveMarkers.Remove(Target);
		return MarkerWidget;
	}
	return nullptr;
}

void UGZMarkerManager::BeginDestroy()
{
	UObject::BeginDestroy();
	//StopMarking();
}

void UGZMarkerManager::StartMarking()
{
	if (TimerHandle.IsValid())return;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::UpdateActiveMarkers, MarkingRate, true);
}

void UGZMarkerManager::StopMarking()
{
	if (TimerHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UGZMarkerManager::UpdateActiveMarkers()
{
	for (auto& Marker : ActiveMarkers)
	{
		if (Marker.Key.IsValid())
		{
			Marker.Value->UpdateMarkerPosition();
		}
	}
}
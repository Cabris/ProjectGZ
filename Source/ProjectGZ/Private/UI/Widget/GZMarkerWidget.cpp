#include "UI/Widget/GZMarkerWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

UGZMarkerWidget::UGZMarkerWidget()
{
	SetMarkerVisibility(false);
	MarkingRate = 30;
	CachedScreenPosition = FVector2D::ZeroVector;
	ScreenEdgeBuffer = 50;
}

void UGZMarkerWidget::SetTarget(AActor* Target, APlayerController* PC)
{
	TargetActor = Target;
	PlayerController = PC;
	OnTargetSet();
}

void UGZMarkerWidget::OnGetFromPool_Implementation()
{
	SetMarkerVisibility(true);
	StartMarking();
}

void UGZMarkerWidget::ResetObjectState_Implementation()
{
	TargetActor = nullptr;
	PlayerController = nullptr;
	SetMarkerVisibility(false);
}

void UGZMarkerWidget::OnReturnToPool_Implementation()
{
	StopMarking();
}

void UGZMarkerWidget::SetMarkerVisibility(bool bVisible)
{
	if (bIsCurrentlyVisible == bVisible)
	{
		return;
	}

	bIsCurrentlyVisible = bVisible;

	const ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	SetVisibility(NewVisibility);
}

void UGZMarkerWidget::StartMarking()
{
	if (TimerHandle.IsValid())return;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::DoMarking, MarkingRate, true);
}

void UGZMarkerWidget::StopMarking()
{
	if (TimerHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void UGZMarkerWidget::DoMarking()
{
	if (TargetActor.IsValid())
		UpdateMarkerPosition();
	else
	{
		StopMarking();
		SetMarkerVisibility(false);
	}
}

void UGZMarkerWidget::UpdateMarkerPosition()
{
	auto WorldPosition = TargetActor->GetActorLocation();
	FVector2D ScreenPosition;
	APlayerController* PC = nullptr;
	if (PlayerController.IsValid())
	{
		PC = PlayerController.Get();
	}
	else
	{
		PC = GetWorld()->GetFirstPlayerController();
	}
	if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WorldPosition, ScreenPosition, true))
	{
		CachedScreenPosition = ScreenPosition;
	}
	bool bIsOnScreen = IsPositionOnScreen(ScreenPosition);
	SetMarkerVisibility(bIsOnScreen);
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
	if (CanvasSlot && bIsOnScreen)
	{
		CanvasSlot->SetPosition(CachedScreenPosition);
	}
}

bool UGZMarkerWidget::IsPositionOnScreen(FVector2D ScreenPosition) const
{
	if (ScreenPosition == FVector2D::ZeroVector)
	{
		return false;
	}

	// 獲取viewport大小
	FVector2D ViewportSize;
	if (PlayerController.Get())
	{
		int32 X, Y;
		PlayerController->GetViewportSize(X, Y);
		ViewportSize = FVector2D(X, Y);
	}
	else if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	else
	{
		return false;
	}

	// 考慮邊緣緩衝區
	const float LeftBound = ScreenEdgeBuffer;
	const float RightBound = ViewportSize.X - ScreenEdgeBuffer;
	const float TopBound = ScreenEdgeBuffer;
	const float BottomBound = ViewportSize.Y - ScreenEdgeBuffer;

	return CachedScreenPosition.X >= LeftBound &&
		CachedScreenPosition.X <= RightBound &&
		CachedScreenPosition.Y >= TopBound &&
		CachedScreenPosition.Y <= BottomBound;
}

#include "UI/Widget/GZMarkerWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "ProjectGZ/ProjectGZ.h"

UGZMarkerWidget::UGZMarkerWidget()
{
	SetMarkerVisibility(false);
	CachedScreenPosition = FVector2D::ZeroVector;
	ScreenEdgeBuffer = 50;
}

void UGZMarkerWidget::SetTarget(AActor* Target, APlayerController* PC)
{
	TargetActor = Target;
	PlayerController = PC;
	OnTargetSet();
}

void UGZMarkerWidget::SetupCanvasSlot(UCanvasPanelSlot* CanvasSlot)
{
	if (Slot && Slot.Get() != CanvasSlot)
	{
		Debug::Print(TEXT("UGZMarkerWidget::SetupCanvasSlot: CanvasSlot is not Current Slot!"));
		return;
	}

	if (IsValid(CanvasSlot))
	{
		CanvasSlot->SetAlignment(SlotConfig.SlotAlignment);
		const FVector4f Anchors = SlotConfig.SlotAnchors;
		CanvasSlot->SetAnchors(FAnchors(Anchors.X, Anchors.Y, Anchors.Z, Anchors.W));
		CanvasSlot->SetZOrder(SlotConfig.SlotZOrder);
	}
}

void UGZMarkerWidget::OnGetFromPool_Implementation()
{
	SetMarkerVisibility(true);
}

void UGZMarkerWidget::ResetObjectState_Implementation()
{
	TargetActor = nullptr;
	PlayerController = nullptr;
	CachedScreenPosition = FVector2D::ZeroVector;
}

void UGZMarkerWidget::OnReturnToPool_Implementation()
{
	SetMarkerVisibility(false);
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

void UGZMarkerWidget::UpdateMarkerPosition()
{
	auto WorldPosition = TargetActor->GetActorLocation();
	FVector2D ScreenPosition;
	APlayerController* PC = nullptr;
	if (PlayerController.IsValid())
	{
		PC = PlayerController.Get();
	}
	else if (GetWorld())
	{
		PC = GetWorld()->GetFirstPlayerController();
	}
	else
	{
		Debug::Print(TEXT("UGZMarkerWidget::UpdateMarkerPosition: PlayerController: Null"));
		return;
	}
	if (UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PC, WorldPosition, ScreenPosition, true))
	{
		CachedScreenPosition = ScreenPosition;
	}

	//bool bIsOnScreen = IsPositionOnScreen(ScreenPosition);
	bool bIsOnScreen = true;
	//SetMarkerVisibility(bIsOnScreen);
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

#pragma once
#include "CoreMinimal.h"
#include "GZUserWidget.h"
#include "Interfactions/GZPoolableObject.h"
#include "GZMarkerWidget.generated.h"

class UCanvasPanelSlot;

UENUM(BlueprintType)
enum EMarkerDisplayType : uint8
{
	MarkerOnScreen,
	MarkerOffScreen
};

USTRUCT(BlueprintType)
struct FSlotConfig
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadOnly, Category="Slot")
	int32 SlotZOrder = 0;
	UPROPERTY(BlueprintReadOnly, Category="Slot")
	FVector4f SlotAnchors = FVector4f(0.0f, 0.0f, 0.0f, 0.0f); //MinX, MinY, MaxX, MaxY
	UPROPERTY(BlueprintReadOnly, Category="Slot")
	FVector2D SlotAlignment = FVector2D(0.5f, 0.5f);
};

UCLASS()
class PROJECTGZ_API UGZMarkerWidget : public UGZUserWidget, public IGZPoolableObject
{
	GENERATED_BODY()

public:
	UGZMarkerWidget();
	virtual void OnGetFromPool_Implementation() override;
	virtual void OnReturnToPool_Implementation() override;
	virtual void ResetObjectState_Implementation() override;
	void SetTarget(AActor* Target, APlayerController* PC);
	UFUNCTION(BlueprintCallable)
	void SetupCanvasSlot(UCanvasPanelSlot* CanvasSlot);

	UFUNCTION(BlueprintCallable)
	void SetMarkerVisibility(bool bVisible);

	UPROPERTY(BlueprintReadOnly, Category="Slot")
	FSlotConfig SlotConfig;

	UFUNCTION(BlueprintPure)
	APlayerController* GetPlayerController() const
	{
		return PlayerController.Get();
	}

	UFUNCTION(BlueprintPure)
	bool IsTargetActorValid() const
	{
		return TargetActor.IsValid();
	}

	UFUNCTION(BlueprintPure)
	AActor* GetTargetActor() const
	{
		return TargetActor.Get();
	}

	void UpdateMarkerPosition();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetSet();
	// UFUNCTION(BlueprintImplementableEvent)
	// void OnMarkerDisplayChanged(EMarkerDisplayType DisplayType);

	// UPROPERTY(BlueprintReadWrite, Category = "MarkerWidget", meta = (BindWidget))
	// TObjectPtr<UTexture2D> MarkerIcon;
	// UPROPERTY(BlueprintReadWrite, Category = "MarkerWidget", meta = (BindWidget))
	// TObjectPtr<UTextBlock> MarkerTitle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MarkerWidget")
	float ScreenEdgeBuffer;

private:

	bool IsPositionOnScreen(FVector2D ScreenPosition) const;
	bool bIsCurrentlyVisible = true;
	FVector2D CachedScreenPosition;
	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerController> PlayerController;
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> TargetActor;
};

#pragma once
#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "GZInteractionDetector.generated.h"

class IGZInteractable;
class UGameplayAbility;
class AActor;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFocusChangedSingnature, AActor* ,NewFocus, AActor* ,OldFocus);

USTRUCT(BlueprintType)
struct FGZInteractionDetectorConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float TraceRadius = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float TraceDistance = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection", meta = (ClampMin = "0.01"))
	float TraceInterval = 0.08f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	float AimWeight = 0.7f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	float DistanceWeight = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bEnableDebugDraw = false;
};

/**
 * 可重用的互動檢測器
 * 負責掃描可互動物件並管理焦點狀態
 */
UCLASS(BlueprintType, Blueprintable)
class PROJECTGZ_API UGZInteractionDetector : public UObject
{
	GENERATED_BODY()

public:
	UGZInteractionDetector();

	// 初始化檢測器
	UFUNCTION(BlueprintCallable, Category = "Interaction Detector")
	void Initialize(UWorld* World, AActor* OwnerActor, const FGZInteractionDetectorConfig& InConfig = FGZInteractionDetectorConfig());

	// 開始檢測
	UFUNCTION(BlueprintCallable, Category = "Interaction Detector")
	void StartDetection();

	// 停止檢測
	UFUNCTION(BlueprintCallable, Category = "Interaction Detector")
	void StopDetection();

	// 是否正在檢測
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction Detector")
	bool IsDetectionActive() const { return TraceTimerHandle.IsValid(); }

	// 獲取當前焦點物件
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction Detector")
	AActor* GetCurrentFocusActor() const { return CurrentFocusActor.Get(); }

	// 強制更新檢測（可用於即時檢測）
	UFUNCTION(BlueprintCallable, Category = "Interaction Detector")
	void ForceUpdate();

	// 更新配置
	UFUNCTION(BlueprintCallable, Category = "Interaction Detector")
	void UpdateConfig(const FGZInteractionDetectorConfig& NewConfig);

	// 事件委託
	UPROPERTY(BlueprintAssignable, Category = "Interaction Detector")
	FOnFocusChangedSingnature OnFocusChanged;

protected:
	UPROPERTY()
	FGZInteractionDetectorConfig Config;

	UPROPERTY()
	TWeakObjectPtr<UWorld> CachedWorld;

	UPROPERTY()
	TWeakObjectPtr<AActor> OwnerActor;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentFocusActor;

	FTimerHandle TraceTimerHandle;
	TArray<FHitResult> HitResults;

	// 執行檢測邏輯
	void PerformDetection();

	// 獲取視點資訊
	virtual bool GetViewPoint(FVector& OutLocation, FVector& OutDirection) const;

	// 評分候選目標
	virtual float ScoreCandidate(const FVector& ViewLocation, const FVector& ViewDirection, const FVector& TargetLocation) const;

	// 檢查物件是否可互動
	virtual bool IsActorInteractable(AActor* Actor) const;

	// 處理焦點變更
	virtual void HandleFocusChange(AActor* NewFocus);

	// 清理資源
	virtual void Cleanup();

public:
	// UObject interface
	virtual void BeginDestroy() override;
};
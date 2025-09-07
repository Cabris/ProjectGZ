#pragma once

#include "CoreMinimal.h"
#include "Game/GZObjectPool.h"
#include "UObject/Object.h"
#include "GZMarkerManager.generated.h"

class UCanvasPanel;
class UGZMarkerWidget;

USTRUCT(BlueprintType)
struct PROJECTGZ_API FGZMarkerConfig
{
	GENERATED_BODY()
	/** 標示 Widget 類別 */
	UPROPERTY(EditDefaultsOnly, Category = "Marker Config")
	TSubclassOf<UGZMarkerWidget> MarkerWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Marker Config")
	int32 ZOrder=0;
};

UCLASS(Blueprintable, BlueprintType)
class PROJECTGZ_API UGZMarkerManager : public UObject
{
	GENERATED_BODY()
	UGZMarkerManager();

public:
	void Initialize(const FGZMarkerConfig& Config, APlayerController* PC);
	UGZMarkerWidget* AddMarkerTarget(AActor* Target);
	UGZMarkerWidget*  RemoveMarkerTarget(AActor* Target);

virtual void BeginDestroy() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "MarkerManager")
	FGZObjectPoolConfig PoolConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MarkerManager")
	float MarkingRate;
private:
	void UpdateActiveMarkers();
	void StartMarking();
	void StopMarking();
	
	bool bIsInitialized;
	UPROPERTY(Transient)
	FGZMarkerConfig MarkerConfig;
	UPROPERTY(Transient)
	TObjectPtr<UGZObjectPool> ObjectPool;
	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<AActor>, UGZMarkerWidget*> ActiveMarkers;
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController;
	FTimerHandle TimerHandle;

};

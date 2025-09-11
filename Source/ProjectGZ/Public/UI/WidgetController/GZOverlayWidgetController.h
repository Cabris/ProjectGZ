#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GZMarkerManager.h"
#include "GZWidgetController.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Data/UIWidgetDataRow.h"
#include "GZOverlayWidgetController.generated.h"

class UGZMarkerManager;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSingnature, float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSingnature, float, NewMaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageWidgetRowSingnature, const FUIWidgetDataRow&, Row);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMarkerWidgetChangeSingnature, UGZMarkerWidget*, MarkerWidget);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusActorChangedSingnature, AActor*, TargetActor);


UCLASS()
class PROJECTGZ_API UGZOverlayWidgetController : public UGZWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="WidgetController|Attributes")
	FOnHealthChangedSingnature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category="WidgetController|Attributes")
	FOnMaxHealthChangedSingnature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category="WidgetController|Message")
	FOnMessageWidgetRowSingnature OnMessageWidgetRow;

	UPROPERTY(BlueprintAssignable, Category="WidgetController|Marker")
	FOnMarkerWidgetChangeSingnature OnMarkerWidgetAdded;
	UPROPERTY(BlueprintAssignable, Category="WidgetController|Marker")
	FOnMarkerWidgetChangeSingnature OnMarkerWidgetRemoved;

	// UPROPERTY(BlueprintAssignable, Category="Interaction")
	// FOnFocusActorChangedSingnature OnActorFocused;
	// UPROPERTY(BlueprintAssignable, Category="Interaction")
	// FOnFocusActorChangedSingnature OnActorUnfocused;

protected:
	virtual void OnWidgetControllerParamsSet() override;

	UPROPERTY(EditDefaultsOnly, Category="WidgetController|Message")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	UPROPERTY(EditDefaultsOnly, Category="WidgetController|Marker")
	TSubclassOf<UGZMarkerManager> MarkerManagerClass;
	UPROPERTY(EditDefaultsOnly, Category="WidgetController|Marker")
	FGZMarkerConfig MarkerConfig;

private:
	void OnMessage(FGameplayTag Channel, const struct FGZVerbMessage& Payload);
	UPROPERTY(Transient)
	TObjectPtr<UGZMarkerManager> MarkerManager;

	//根據傳入的表格和Tag返回尋找到的資料，表格類型不確定，所以使用T來表示，在使用此函數時，需要指定對應類型
	// template <typename T>
	// T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	FGameplayMessageListenerHandle EffectAppliedMessageListenerHandle;
};

// template <typename T>
// T* UGZOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
// {
// 	T* Row = DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
// 	return Row;
// }

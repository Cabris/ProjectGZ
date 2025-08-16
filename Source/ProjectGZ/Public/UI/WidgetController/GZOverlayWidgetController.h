#pragma once
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GZWidgetController.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Data/UIWidgetDataRow.h"
#include "GZOverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSingnature, float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSingnature, float, NewMaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMessageWidgetRowSingnature, const FUIWidgetDataRow&, Row);

UCLASS()
class PROJECTGZ_API UGZOverlayWidgetController : public UGZWidgetController
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnHealthChangedSingnature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnMaxHealthChangedSingnature OnMaxHealthChanged;
	UPROPERTY(BlueprintAssignable, Category="Messages")
	FOnMessageWidgetRowSingnature OnMessageWidgetRow;

protected:
	void HealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	void MaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	UPROPERTY(EditDefaultsOnly, Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

private:
	//根據傳入的表格和Tag返回尋找到的資料，表格類型不確定，所以使用T來表示，在使用此函數時，需要指定對應類型
	template <typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	FGameplayMessageListenerHandle EffectAppliedMessageListenerHandle;
};

template <typename T>
T* UGZOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	T* Row = DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
	return Row;
}

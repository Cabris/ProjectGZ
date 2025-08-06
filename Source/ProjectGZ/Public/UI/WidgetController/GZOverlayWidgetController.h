#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GZWidgetController.h"
#include "GZOverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSingnature, float, NewHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSingnature, float, NewMaxHealth);

UCLASS()
class PROJECTGZ_API UGZOverlayWidgetController : public UGZWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnHealthChangedSingnature OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnMaxHealthChangedSingnature OnMaxHealthChanged;

protected:
	void HealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	void MaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;

private:
 
};

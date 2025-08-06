#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GZWidgetController.generated.h"

class AGZPlayerController;
class AGZPlayerState;
class UGZAbilitySystemComponent;
class UGZAttributeSet;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	FWidgetControllerParams() = default;

	FWidgetControllerParams(AGZPlayerController* PC, AGZPlayerState* PS, UGZAbilitySystemComponent* ASC, UGZAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AGZPlayerController> PlayerController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AGZPlayerState> PlayerState = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UGZAttributeSet> AttributeSet = nullptr;
};


UCLASS(BlueprintType, Blueprintable)
class PROJECTGZ_API UGZWidgetController : public UObject
{
	GENERATED_BODY()

public:
	void SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams);
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<AGZPlayerController> PlayerController = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<AGZPlayerState> PlayerState = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<UGZAbilitySystemComponent> AbilitySystemComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, Category="Widget Controller")
	TObjectPtr<UGZAttributeSet> AttributeSet = nullptr;

private:
};

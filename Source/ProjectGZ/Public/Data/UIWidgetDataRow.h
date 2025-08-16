#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UIWidgetDataRow.generated.h"

class UGZUserWidget;

USTRUCT(BlueprintType)
struct FUIWidgetDataRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag MessageTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Message;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGZUserWidget> MessageWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UTexture2D* Icon = nullptr;
};
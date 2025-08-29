#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GZDataAssetInputConfig.generated.h"
class UInputAction;

USTRUCT(BlueprintType)
struct FInputActionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ActionConfig")
	UInputAction* InputAction = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ActionConfig")
	FGameplayTag InputTag;
};

class UInputMappingContext;

UCLASS()
class PROJECTGZ_API UGZDataAssetInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(Blueprintable)
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InputTag) const;

	const UInputMappingContext* GetInputMappingContext() const
	{
		return DefaultMappingContext;
	}

protected:
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, meta = (TitleProperty = "InputTag"))
	TArray<FInputActionConfig> NativeInputActions;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (TitleProperty = "InputTag"))
	TArray<FInputActionConfig> AbilityInputActions;

public:
	const TArray<FInputActionConfig>& GetNativeInputActions() const
	{
		return NativeInputActions;
	}

	const TArray<FInputActionConfig>& GetAbilityInputActions() const
	{
		return AbilityInputActions;
	}
};

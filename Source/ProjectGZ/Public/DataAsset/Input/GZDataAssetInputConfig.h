#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset/DataAssetStructs.h"
#include "GZDataAssetInputConfig.generated.h"

/**
 * 
 */
class UInputMappingContext;

UCLASS()
class PROJECTGZ_API UGZDataAssetInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "InputTag"))
	TArray<FInputActionConfig> NativeInputActions;
	UFUNCTION(Blueprintable)
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InputTag)const;
};

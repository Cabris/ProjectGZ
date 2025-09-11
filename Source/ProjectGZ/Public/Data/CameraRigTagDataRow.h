#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CameraRigTagDataRow.generated.h"

class UCameraRigAsset;

USTRUCT(BlueprintType)
struct FCameraRigTagDataRow : public FTableRowBase
{
GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CameraRigTag;
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCameraRigAsset> TestCameraRig;
};
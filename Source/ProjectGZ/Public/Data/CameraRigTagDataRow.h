#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CameraRigTagDataRow.generated.h"


USTRUCT(BlueprintType)
struct FCameraRigTagDataRow : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CameraRigTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector ArmLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ArmLength;
};

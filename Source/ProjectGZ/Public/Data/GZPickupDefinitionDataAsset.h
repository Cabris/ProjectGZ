#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GZPickupDefinitionDataAsset.generated.h"

class UGZInventoryItemDefinition;
//DA for item spawner to config and spawn pickupable item.
UCLASS(Blueprintable,BlueprintType)
class PROJECTGZ_API UGZPickupDefinitionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//Cool down time between pickups in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	int32 SpawnCoolDownSeconds;
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TSubclassOf<UGZInventoryItemDefinition> PickupDefinitionClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMesh> DisplayMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	FTransform DisplayMeshTransformOffset;
};

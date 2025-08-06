// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GZPickupDefinitionDataAsset.generated.h"

class UGZEquipmentDefinition;

//DA for item spawner to config and spawn pickupable item.
UCLASS()
class PROJECTGZ_API UGZPickupDefinitionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	//Cool down time between pickups in seconds
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	int32 SpawnCoolDownSeconds;
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	TSubclassOf<UGZEquipmentDefinition> PickupDefinitionClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UStaticMesh> DisplayMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	FTransform DisplayMeshTransformOffset;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssetStructs.h"
#include "Structs/GZPrimaryDataAssetBase.h"
#include "GZRoadMeshDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGZ_API UGZRoadMeshDataAsset : public UGZPrimaryDataAssetBase
{
	GENERATED_BODY()
public:
	UGZRoadMeshDataAsset();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Road Items")
	TArray<FRoadItemStruct> PlaceableItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Road Items")
	TArray<FRoadCurvedStruct> CurvedItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Road Items")
	TArray<FRoadCurvedStruct> ChainedItems;
};

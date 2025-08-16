// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GZPrimaryDataAssetBase.h"
#include "DataAssetStructs.h"
#include "GZBuildingPrimaryDataAsset.generated.h"

UCLASS()
class PROJECTGZ_API UGZBuildingPrimaryDataAsset : public UGZPrimaryDataAssetBase
{
	GENERATED_BODY()
public:
	UGZBuildingPrimaryDataAsset();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	TArray<FBuildingStruct> Buildings;
};

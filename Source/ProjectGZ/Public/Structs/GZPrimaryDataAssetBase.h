// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GZPrimaryDataAssetBase.generated.h"

UCLASS()
class PROJECTGZ_API UGZPrimaryDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Assets")
	FPrimaryAssetType Type;
	virtual  FPrimaryAssetId GetPrimaryAssetId() const override;
};

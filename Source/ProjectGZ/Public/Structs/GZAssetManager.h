// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GZAssetManager.generated.h"

UCLASS()
class PROJECTGZ_API UGZAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	virtual  void StartInitialLoading() override;
	static const FPrimaryAssetType DynamicMeshDataAsset;
	static const FPrimaryAssetType RoadMeshDataAsset;
};

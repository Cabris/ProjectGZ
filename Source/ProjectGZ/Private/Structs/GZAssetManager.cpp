// Fill out your copyright notice in the Description page of Project Settings.
#include "Structs/GZAssetManager.h"

const FPrimaryAssetType UGZAssetManager::DynamicMeshDataAsset=TEXT("DynamicMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::RoadMeshDataAsset=TEXT("RoadMeshDataAsset");

void UGZAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

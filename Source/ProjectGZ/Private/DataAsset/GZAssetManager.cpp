// Fill out your copyright notice in the Description page of Project Settings.
#include "DataAsset/GZAssetManager.h"

const FPrimaryAssetType UGZAssetManager::DynamicMeshDataAsset=TEXT("DynamicMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::RoadMeshDataAsset=TEXT("RoadMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::BuildingDataAsset=TEXT("BuildingDataAsset");

void UGZAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}

#include "Game/GZAssetManager.h"
#include "Game/GZGameplayTags.h"

const FPrimaryAssetType UGZAssetManager::DynamicMeshDataAsset = TEXT("DynamicMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::RoadMeshDataAsset = TEXT("RoadMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::BuildingDataAsset = TEXT("BuildingDataAsset");

UGZAssetManager& UGZAssetManager::Get()
{
	check(GEngine);
	UGZAssetManager* AM = Cast<UGZAssetManager>(GEngine->AssetManager);
	check(AM);
	return  *AM;
}

void UGZAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FGZGameplayTags::Init();
}

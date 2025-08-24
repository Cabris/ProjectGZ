#include "Game/GZAssetManager.h"
#include "Game/GZGameplayTags.h"

const FPrimaryAssetType UGZAssetManager::DynamicMeshDataAsset = TEXT("DynamicMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::RoadMeshDataAsset = TEXT("RoadMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::BuildingDataAsset = TEXT("BuildingDataAsset");
PRAGMA_DISABLE_OPTIMIZATION
UGZAssetManager& UGZAssetManager::Get()
{
	check(GEngine);
	auto Temp = GEngine->AssetManager;
	check(Temp);
	UGZAssetManager* AM = Cast<UGZAssetManager>(Temp);
	check(AM);
	return *AM;
}
PRAGMA_ENABLE_OPTIMIZATION
void UGZAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	//FGZGameplayTags::Init();
}

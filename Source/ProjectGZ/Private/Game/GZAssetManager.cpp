#include "Game/GZAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "Game/GZGameplayCueManager.h"
#include "Game/GZGameplayTags.h"

const FPrimaryAssetType UGZAssetManager::DynamicMeshDataAsset = TEXT("DynamicMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::RoadMeshDataAsset = TEXT("RoadMeshDataAsset");
const FPrimaryAssetType UGZAssetManager::BuildingDataAsset = TEXT("BuildingDataAsset");
UE_DISABLE_OPTIMIZATION

UGZAssetManager& UGZAssetManager::Get()
{
	check(GEngine);
	auto Temp = GEngine->AssetManager;
	check(Temp);
	UGZAssetManager* AM = Cast<UGZAssetManager>(Temp);
	check(AM);
	return *AM;
}

UE_ENABLE_OPTIMIZATION

void UGZAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UGZGameplayCueManager* GCM = UGZGameplayCueManager::Get();
	if (GCM)
		GCM->StartInitialLoading();
	//如果使用TargetData，必須開啟此項
	//UAbilitySystemGlobals::Get().InitGlobalData();
	//FGZGameplayTags::Init();
}

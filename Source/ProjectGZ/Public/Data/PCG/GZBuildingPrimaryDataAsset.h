#pragma once
#include "CoreMinimal.h"
#include "Data/GZPrimaryDataAssetBase.h"
#include "GZBuildingPrimaryDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FBuildingStruct
{
	GENERATED_BODY()

	// 要放置的靜態 Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> BuildingMesh;

	// 用於調整 cell 大小的權重 (越大，Voronoi 分割後該 cell 面積越大)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GenerationWeight = 1.0f;

	// 這棟建築是否允許旋轉(yaw)已符合擺放空間
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCanRotate = true;
};

UCLASS()
class PROJECTGZ_API UGZBuildingPrimaryDataAsset : public UGZPrimaryDataAssetBase
{
	GENERATED_BODY()
public:
	UGZBuildingPrimaryDataAsset();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Building")
	TArray<FBuildingStruct> Buildings;
};

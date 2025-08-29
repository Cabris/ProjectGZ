#pragma once
#include "CoreMinimal.h"
#include "Data/GZPrimaryDataAssetBase.h"
#include "GZDynamicMeshDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FDynamicMeshStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSoftObjectPtr<UStaticMesh> StaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	float Weight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FVector Scale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FRotator Rotation = FRotator::ZeroRotator;
};

UCLASS()
class PROJECTGZ_API UGZDynamicMeshDataAsset : public UGZPrimaryDataAssetBase
{
	GENERATED_BODY()

public:
	UGZDynamicMeshDataAsset();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MeshGroup")
	TArray<FDynamicMeshStruct> MeshVariations;
	// 可在 Blueprint 編輯器裡新增多筆不同 Struct 變體
	//TArray<FInstancedStruct> MeshVariations;
};

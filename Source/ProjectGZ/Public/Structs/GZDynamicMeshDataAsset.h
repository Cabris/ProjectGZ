// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/GZPrimaryDataAssetBase.h"
//#include "StructUtils/InstancedStruct.h"
#include  "Structs/DataAssetStructs.h"
#include "GZDynamicMeshDataAsset.generated.h"

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

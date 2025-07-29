#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataAssetStructs.generated.h"

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

USTRUCT(BlueprintType)
struct FRoadItemStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	float StartOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	float Interval = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	TSoftObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	FTransform MeshTransform = FTransform(
		FRotator(0, 0, 0),
		FVector(0, 0, 0),
		FVector(1, 1, 1)
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	FTransform TransformRandom = FTransform(
		FRotator(0, 0, 0),
		FVector(0, 0, 0),
		FVector(0, 0, 0)
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool KeepVertical = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool HasMirror = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool ReverseMirror = false;
};

USTRUCT(BlueprintType)
struct FRoadCurvedStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	TSoftObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	FVector MeshPosition = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	FVector PositionOffset = FVector::Zero();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	bool HasMirror = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	bool ReverseMirror = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	int ForwardAxis = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	FVector UpVector = FVector::UpVector;
};

USTRUCT(BlueprintType)
struct FChainedItemStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	float StartOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	TSoftObjectPtr<UStaticMesh> Mesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	FTransform MeshTransform = FTransform(
		FRotator(0, 0, 0),
		FVector(0, 0, 0),
		FVector(1, 1, 1)
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	FTransform TransformRandom = FTransform(
		FRotator(0, 0, 0),
		FVector(0, 0, 0),
		FVector(0, 0, 0)
	);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool KeepVertical = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool HasMirror = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool ReverseMirror = false;
};

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

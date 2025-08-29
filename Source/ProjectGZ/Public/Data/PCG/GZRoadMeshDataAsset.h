// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectGZ/Public/Data/GZPrimaryDataAssetBase.h"
#include "GZRoadMeshDataAsset.generated.h"

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

UCLASS()
class PROJECTGZ_API UGZRoadMeshDataAsset : public UGZPrimaryDataAssetBase
{
	GENERATED_BODY()
public:
	UGZRoadMeshDataAsset();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Road Items")
	TArray<FRoadItemStruct> PlaceableItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Road Items")
	TArray<FRoadCurvedStruct> CurvedItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Road Items")
	TArray<FChainedItemStruct> ChainedItems;
};

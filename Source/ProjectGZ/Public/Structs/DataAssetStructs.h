#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DataAssetStructs.generated.h"


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
	bool KeepVertical;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool HasMirror;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Item")
	bool ReverseMirror;
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
	bool HasMirror;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	bool ReverseMirror;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	int ForwardAxis = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Road Curved")
	FVector UpVector = FVector::UpVector;
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

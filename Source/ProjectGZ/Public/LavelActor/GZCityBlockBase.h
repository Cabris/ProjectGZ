// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GZCityBlockBase.generated.h"

class USplineComponent;
class UPCGComponent;
UCLASS()
class PROJECTGZ_API AGZCityBlockBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGZCityBlockBase();
	UPROPERTY(EditAnywhere, Category="CityBlock")
	TObjectPtr<USplineComponent> Spline;
	UPROPERTY(EditAnywhere, Category="CityBlock")
	TObjectPtr<UPCGComponent> PCG;
	UPROPERTY(EditAnywhere, Category="CityBlock")
	FString CityBlockId;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

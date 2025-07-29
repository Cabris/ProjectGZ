// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SplineFunctionLibrary.generated.h"

/**
 * 
 */
class USplineComponent;
UCLASS()
class PROJECTGZ_API USplineFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void UpdateOverrideComponent(UPARAM(ref) USplineComponent* spline);
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GZFunctionLibrary.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class GZ_HELPER_API UGZFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "GZHelper")
	static void GZHelper_NotifySplineUpdate(UPARAM(ref) USplineComponent* Spline);

	UFUNCTION(BlueprintCallable, Category = "GZHelper")
	static AActor* GZHelper_GetActorFromSotfPath(const FSoftObjectPath& SoftObjectPath);

	UFUNCTION(BlueprintCallable, Category = "GZHelper")
	static bool GZHelper_GetViewPointFromActor(const AActor* Actor, FVector& OutLocation, FVector& OutDirection);

	
};

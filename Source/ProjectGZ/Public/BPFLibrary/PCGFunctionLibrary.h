// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PCGFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGZ_API UPCGFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//UFUNCTION(BlueprintCallable, Category = "Path")
	//static FPCGPoint FindCenter(TArray<FPCGPoint> PathPoints);


	UFUNCTION(BlueprintCallable, Category = "GZ")
	static AActor* GetActorFromSotfPath(const FSoftObjectPath& SoftObjectPath);
};

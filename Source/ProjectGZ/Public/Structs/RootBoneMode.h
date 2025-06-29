// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RootBoneMode.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class  ERootBoneMode :uint8
{
	//GENERATED_BODY()
	Accumulate UMETA(DisplayName="Accumulate"),
	BlendOut UMETA(DisplayName="BlendOut"),
	Hold UMETA(DisplayName="Hold")
};

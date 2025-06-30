// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TurnPoses.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETurnPoses :uint8
{
	TurnCW UMETA(DisplayName="Turn Clockwise"),
	TurnCCW UMETA(DisplayName="Turn Counterclockwise"),
	NoTurn UMETA(DisplayName="No Turn"),
};

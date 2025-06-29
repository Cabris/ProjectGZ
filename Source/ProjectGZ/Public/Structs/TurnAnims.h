// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TurnAnims.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FTurnAnims
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnRight90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnRight180Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnLeft90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnLeft180Anim;
};

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
	TObjectPtr<UAnimSequence> TurnRight90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequence> TurnRight180Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequence> TurnLeft90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequence> TurnLeft180Anim;
};

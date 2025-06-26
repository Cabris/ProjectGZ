// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GZCharacterAnimInstance.h"
#include "GZPlayerCharacterAnimInst.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTGZ_API UGZPlayerCharacterAnimInst : public UGZCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnRight90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnRight180Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnLeft90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequenceBase> TurnLeft180Anim;

	UFUNCTION(BlueprintCallable)
	UAnimSequenceBase* GetTurnAnim(float deltaAngle);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DirectionAngle; //angle between velocity and facing
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector DesiredDirection; //world space direction that move input axis vector 
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float DeltaAngle; //angle between DesiredDirection and facing
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bShouldTurn;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	bool bHasMovementInput;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FVector LastMovementInputVector;
	UPROPERTY(BlueprintReadOnly, Category = "Input")
	bool bIsStandToMove;
	UPROPERTY(EditAnywhere, Category = "Input")
	float StandToMoveThreshold;

private:
	//bool bIsFirstFrame;
	float ElapsedTimeSinceMovementInput;
};

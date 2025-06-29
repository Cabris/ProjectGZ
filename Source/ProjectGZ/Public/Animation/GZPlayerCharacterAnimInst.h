// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GZCharacterAnimInstance.h"
#include "Structs/RootBoneMode.h"
#include "Structs/TurnAnims.h"
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

	///RootMotionTurn
	UFUNCTION(BlueprintCallable)
	UAnimSequenceBase* GetTurnAnim(float deltaAngle);
	UPROPERTY(EditAnywhere, Category = "Animations")
	FTurnAnims TurnAnims;
	UPROPERTY(EditAnywhere, Category = "Animations")
	ERootBoneMode RootBoneMode;

	UFUNCTION(BlueprintCallable)
	void UpdateRootYawOffetData(float deltaTime);

	UFUNCTION(BlueprintCallable)
	void UpdateActorYawDelta();

	UFUNCTION(BlueprintCallable)
	void SetRootYawOffset(float newRootYawOffest);

	UFUNCTION(BlueprintCallable)
	void ProcessTurnYawCurve();

	float LastRootYawOffet;
	float RootYawOffset;

	float ActorYawDelta;
	float ActorYaw;
	float LastActorYaw;

	float TurnYawCurveValue;
	float LastTurnYawCurveValue;

	FName TurnYawWeight = TEXT("TurnYawWeight");
	FName RemainingTurnYaw = TEXT("RemainingTurnYaw");
	///
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
	float CurrentTurnAnimTime;
};

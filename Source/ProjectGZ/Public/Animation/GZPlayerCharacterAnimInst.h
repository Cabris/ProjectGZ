// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/GZCharacterAnimInstance.h"
#include "Structs/RootBoneMode.h"
#include "Structs/TurnAnims.h"
#include "Structs/TurnPoses.h"
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
	UAnimSequenceBase* GetTurnAnim();
	UFUNCTION(BlueprintCallable)
	void UpdateRootYawOffetData(float deltaTime);
	UFUNCTION(BlueprintCallable)
	void UpdateActorYawDelta();
	UFUNCTION(BlueprintCallable)
	void SetRootYawOffset(float newRootYawOffest);
	UFUNCTION(BlueprintCallable)
	void ProcessTurnYawCurve();

	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float LastRootYawOffet;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float RootYawOffset;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float ActorYawDelta;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float ActorYaw;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float LastActorYaw;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float TurnYawCurveValue;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float LastTurnYawCurveValue;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	ETurnPoses TurnPose;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	FTurnAnims TurnAnims;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	ERootBoneMode RootBoneMode;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	TObjectPtr<UAnimSequenceBase> FinalTurnAnim;
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float TurnInPlaceAnimTime;

	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	float TurnYawOffsetThreshold = 50;

	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
	FName TurnYawWeight = TEXT("TurnYawWeight");
	UPROPERTY(BlueprintReadWrite, Category = "TurnInPlace")
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

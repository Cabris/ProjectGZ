#include "Animation/GZPlayerCharacterAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GZCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"

void UGZPlayerCharacterAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// bIsFirstFrame = true;
	ElapsedTimeSinceMovementInput = 0;
	StandToMoveThreshold = 0.2f;
	CurrentTurnAnimTime = 0.0f;
	DesiredDirection = FVector::ZeroVector;
}

UAnimSequenceBase* UGZPlayerCharacterAnimInst::GetTurnAnim()
{
	const float turnYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset);
	const float angleThreshold = TurnYawOffsetThreshold;
	if (TurnPose == ETurnPoses::TurnCW)
	{
		if (FMath::Abs(turnYawOffset) > angleThreshold)
			return TurnAnims.TurnRight180Anim;
		else
		{
			return TurnAnims.TurnRight90Anim;
		}
	}
	else if (TurnPose == ETurnPoses::TurnCCW)
	{
		if (FMath::Abs(turnYawOffset) > angleThreshold)
			return TurnAnims.TurnLeft180Anim;
		else
		{
			return TurnAnims.TurnLeft90Anim;
		}
	}
	return nullptr;
}

void UGZPlayerCharacterAnimInst::UpdateRootYawOffetData(float deltaTime)
{
	if (RootBoneMode == ERootBoneMode::Accumulate)
	{
		SetRootYawOffset(RootYawOffset - ActorYawDelta);
	}
	if (RootBoneMode == ERootBoneMode::BlendOut)
	{
		FFloatSpringState state;
		float ret = UKismetMathLibrary::FloatSpringInterp(
			RootYawOffset, 0.0f, state, 80,
			1.0f, deltaTime, 1, 0.5f);
		SetRootYawOffset(ret);
	}
	RootBoneMode = ERootBoneMode::BlendOut;
}

void UGZPlayerCharacterAnimInst::UpdateActorYawDelta()
{
	if (Character)
	{
		const auto ActorRotation = Character->GetActorRotation();
		LastActorYaw = ActorYaw;
		ActorYaw = ActorRotation.Yaw;
		ActorYawDelta = ActorYaw - LastActorYaw;
	}
}

void UGZPlayerCharacterAnimInst::SetRootYawOffset(float newRootYawOffest)
{
	LastRootYawOffet = RootYawOffset;
	RootYawOffset = UKismetMathLibrary::NormalizeAxis(newRootYawOffest);
}

void UGZPlayerCharacterAnimInst::ProcessTurnYawCurve()
{
	LastTurnYawCurveValue = TurnYawCurveValue;
	float weight = GetCurveValue(TurnYawWeight);
	if (weight < 1.0f)
	{
		LastTurnYawCurveValue = TurnYawCurveValue = 0;
	}
	else
	{
		float remainingTurnYaw = GetCurveValue(RemainingTurnYaw);
		float newRemainingYaw = UKismetMathLibrary::SafeDivide(remainingTurnYaw, weight);
		TurnYawCurveValue = newRemainingYaw;
		if (LastTurnYawCurveValue != 0.0f)
		{
			SetRootYawOffset(RootYawOffset - (TurnYawCurveValue - LastTurnYawCurveValue));
		}
	}
}


void UGZPlayerCharacterAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character)
	{
		LastMovementInputVector = CMC->GetLastInputVector();
		bHasMovementInput = LastMovementInputVector.Size() > KINDA_SMALL_NUMBER;
		//increase ElapsedTime
		if (bHasMovementInput)
		{
			ElapsedTimeSinceMovementInput += DeltaSeconds;
			DesiredDirection = LastMovementInputVector;
		}
		else
			ElapsedTimeSinceMovementInput = 0;

		const auto Velocity = Character->GetVelocity();
		const auto ActorRotation = Character->GetActorRotation();
		DirectionAngle = FMath::RoundToFloat(UKismetAnimationLibrary::CalculateDirection(Velocity, ActorRotation));
		DeltaAngle = FMath::RoundToFloat(UKismetAnimationLibrary::CalculateDirection(DesiredDirection, ActorRotation));
		bIsStandToMove = bHasMovementInput && ElapsedTimeSinceMovementInput >= StandToMoveThreshold;
		bShouldTurn = bHasMovementInput && ElapsedTimeSinceMovementInput <= StandToMoveThreshold;

		UpdateActorYawDelta();
		UpdateRootYawOffetData(DeltaSeconds);
	}

	FString Msg = FString::Printf(TEXT(
		"DirectionAngle: %f | DesiredDirection: %s | AngleDeltaDegrees: %f"),
	                              DirectionAngle, *DesiredDirection.ToString(), DeltaAngle);
	GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::Cyan, Msg);

	Msg = FString::Printf(TEXT(
		"bHasMovementInput: %d | LastMovementInputVector: %s | bIsStandToMove: %d"),
	                      bHasMovementInput, *LastMovementInputVector.ToString(), bIsStandToMove);
	GEngine->AddOnScreenDebugMessage(3, 0.0f, FColor::Magenta, Msg);

	if (Character)
	{
		FVector Start = Character->GetActorLocation();
		float Length = 100.0f; // 箭頭長度
		FVector End = Start + DesiredDirection * Length;
		DrawDebugDirectionalArrow(GetWorld(), Start, End, 100.0f, FColor::Blue, false, 0.0f, 1, 2.0f);
	}
}

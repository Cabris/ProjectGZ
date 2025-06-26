#include "Animation/GZPlayerCharacterAnimInst.h"

#include "KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GZCharacterBase.h"

void UGZPlayerCharacterAnimInst::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// bIsFirstFrame = true;
	ElapsedTimeSinceMovementInput = 0;
	StandToMoveThreshold = 0.2f;
	DesiredDirection = FVector::ZeroVector;
}

UAnimSequenceBase* UGZPlayerCharacterAnimInst::GetTurnAnim(float deltaAngle)
{
	if (deltaAngle > 0 && deltaAngle <= 90)
		return TurnRight90Anim;
	if (deltaAngle > 90 && deltaAngle <= 180)
		return TurnRight180Anim;
	if (deltaAngle < 0 && deltaAngle <= -90)
		return TurnLeft90Anim;
	if (deltaAngle < -90 && deltaAngle >= -180)
		return TurnLeft180Anim;
	return nullptr;
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

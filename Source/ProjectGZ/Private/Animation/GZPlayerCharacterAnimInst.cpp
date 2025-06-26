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
}

void UGZPlayerCharacterAnimInst::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character)
	{
		LastMovementInputVector = CMC->GetLastInputVector();
		bHasMovementInput = LastMovementInputVector.Size() > KINDA_SMALL_NUMBER;
		DesiredDirection = bHasMovementInput ? LastMovementInputVector : FVector::ZeroVector;
		const auto Velocity = Character->GetVelocity();
		DirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
		AngleDeltaDegrees = UKismetAnimationLibrary::CalculateDirection(DesiredDirection, Character->GetActorRotation());

		//increase ElapsedTime
		if (bHasMovementInput)
		{
			ElapsedTimeSinceMovementInput += DeltaSeconds;
		}
		else
		{
			ElapsedTimeSinceMovementInput = 0;
		}
		bIsStandToMove = ElapsedTimeSinceMovementInput >= StandToMoveThreshold;
	}

	FString Msg = FString::Printf(TEXT(
		"DirectionAngle: %f | DesiredDirection: %s | AngleDeltaDegrees: %f"),
	                              DirectionAngle, *DesiredDirection.ToString(), AngleDeltaDegrees);
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

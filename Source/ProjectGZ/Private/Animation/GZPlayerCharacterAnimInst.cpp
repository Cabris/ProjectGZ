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
		// if (bIsFirstFrame)
		// {
		// 	bIsFirstFrame = false;
		// }
		LastMovementInputVector = CMC->GetLastInputVector();
		bHasMovementInput = LastMovementInputVector.Size() > KINDA_SMALL_NUMBER;

		const FRotator ControlRot = Character->GetControlRotation();
		const FRotator YawRot(0, ControlRot.Yaw, 0);
		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		DesiredDirection = (ForwardDir * LastMovementInputVector.Y +
			RightDir * LastMovementInputVector.X).GetSafeNormal();

		const auto Velocity = Character->GetVelocity();
		DirectionAngle = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());

		AngleDeltaDegrees = FMath::RadiansToDegrees(
			FMath::Acos(FVector::DotProduct(DesiredDirection, Character->GetActorForwardVector())));

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
}

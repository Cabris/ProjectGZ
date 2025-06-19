#include "Animation/GZCharacterAnimInstance.h"
#include "Character/GZCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGZCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	InitializeCharacter();
	IsFirstFrame = true;
	ElapsedTimeSinceMovementInput = 0;
}

void UGZCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UGZCharacterAnimInstance::InitializeCharacter()
{
	Character = Cast<AGZCharacterBase>(TryGetPawnOwner());
	CMC = Character->GetCharacterMovement();
	//RootMotion take over movement
	CMC->MovementMode = MOVE_None;
	CMC->bUseControllerDesiredRotation = false;
	CMC->bOrientRotationToMovement = false;
	//RootMotionMode = ERootMotionMode::Type::RootMotionFromMontagesOnly; // 或 Everything
}

void UGZCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character)
	{
		auto Velocity = Character->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();
		bIsAccelerating = CMC->GetCurrentAcceleration().Size() > 0.0f;
		bIsInAir = CMC->IsFalling();
		// Character->GetCharacterMovement()->IsCrouching();
		LastMovementInputVector = CMC->GetLastInputVector();
		bHasMovementInput = LastMovementInputVector.Size() < 0.1f;
		
		//Character->GetControlRotation();
		if (IsFirstFrame)
		{
			IsFirstFrame = false;
			ElapsedTimeSinceMovementInput = 0;
		}

		//increase ElapsedTime
		if (bHasMovementInput)
		{
			ElapsedTimeSinceMovementInput += DeltaSeconds;
		}
		else
		{
			ElapsedTimeSinceMovementInput = 0;
		}
	}
	else
	{
		InitializeCharacter();
	}
}

#include "Animation/GZCharacterAnimInstance.h"
#include "Character/GZCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGZCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	InitializeCharacter();
}

void UGZCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UGZCharacterAnimInstance::InitializeCharacter()
{
	Character = Cast<AGZCharacterBase>(TryGetPawnOwner());
	if (Character)
		CMC = Character->GetCharacterMovement();
	//RootMotion take over movement
	// CMC->MovementMode = MOVE_None;
	// CMC->bUseControllerDesiredRotation = false;
	// CMC->bOrientRotationToMovement = false;
	//RootMotionMode = ERootMotionMode::Type::RootMotionFromMontagesOnly; // 或 Everything
}

void UGZCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character && CMC)
	{
		auto Velocity = Character->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();
		Direction = Character->GetActorForwardVector();
		bIsInAir = CMC->IsFalling();
		bIsCrouch = CMC->IsCrouching();
		// bIsAccelerating = CMC->GetCurrentAcceleration().Size() > 0.0f;
	}
	else
	{
		InitializeCharacter();
	}
}

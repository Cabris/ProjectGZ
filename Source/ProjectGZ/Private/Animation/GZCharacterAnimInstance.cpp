#include "Animation/GZCharacterAnimInstance.h"
#include "Character/GZCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

void UGZCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AGZCharacterBase>(TryGetPawnOwner());
}

void UGZCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
}

void UGZCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character)
	{
		auto Velocity = Character->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();
		bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;
		bIsInAir = Character->GetCharacterMovement()->IsFalling();
		// Character->GetCharacterMovement()->IsCrouching();
	}
	else
	{
		Character = Cast<AGZCharacterBase>(TryGetPawnOwner());
	}
}

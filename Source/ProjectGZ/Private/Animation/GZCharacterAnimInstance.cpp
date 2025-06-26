#include "Animation/GZCharacterAnimInstance.h"
#include "Character/GZCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfactions/Strafingable.h"

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

		IStrafingable* Strafingable = Cast<IStrafingable>(Character);
		if (Strafingable)
		{
			bIsStrafing = Strafingable->IsStrafing();
		}
	}
	else
	{
		InitializeCharacter();
	}

	FString Msg = FString::Printf(TEXT("DirectionAngle: %f | Rotation: %s | bIsInAir: %d | bIsCrouch: %d"),
	                              Speed, *Direction.ToString(), bIsInAir, bIsCrouch);

	// 用固定 Key（例如 1）防止洗版，每次更新會取代原本內容
	GEngine->AddOnScreenDebugMessage(
		1, // Key
		0.0f, // Display time（0 表示直到下一次呼叫取代）
		FColor::Green, // 顏色
		Msg
	);

	if (Character)
	{
		FVector Start = Character->GetActorLocation();
		float Length = 100.0f; // 箭頭長度
		FVector End = Start + Direction * Length;

		DrawDebugDirectionalArrow(
			GetWorld(), // 世界參考
			Start, // 起點
			End, // 終點
			100.0f, // 箭頭頭部大小（HeadSize）
			FColor::Green, // 顏色
			false, // bPersistentLines：是否永久存在
			0.0f, // LifeTime：0 為只畫這一幀
			1, // DepthPriority
			2.0f // 線條寬度
		);
	}
}

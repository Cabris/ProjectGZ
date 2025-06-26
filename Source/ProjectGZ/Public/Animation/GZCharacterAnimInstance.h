#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GZCharacterAnimInstance.generated.h"

class AGZCharacterBase;
class UCharacterMovementComponent;

UCLASS()
class PROJECTGZ_API UGZCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<AGZCharacterBase> Character;
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterMovementComponent> CMC;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Direction;	//pawn facing direction
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsCrouch;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsStrafing;
private:
	void InitializeCharacter();
};

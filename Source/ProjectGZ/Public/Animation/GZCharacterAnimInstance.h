#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GZCharacterAnimInstance.generated.h"

class AGZCharacterBase;

UCLASS()
class PROJECTGZ_API UGZCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<AGZCharacterBase> Character;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsInAir;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsAccelerating;

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
};

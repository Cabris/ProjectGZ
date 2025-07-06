#pragma once
#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Structs/CardinalDirection.h"
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


	UFUNCTION(BlueprintCallable, Category="AnimGraph|NodeFunctions",
		meta=(BlueprintThreadSafe))
	void UpdateMyLogic(const FVector& InVelocity, FVector& OutVelocity, float& OutAngle);

	UFUNCTION(BlueprintCallable, Category="AnimGraph|NodeFunctions",
		meta=(BlueprintThreadSafe))
	void UpdateMyLogic2(const FVector InVelocity, FVector& OutVelocity, float& OutAngle);

	// 範例：綁定到 Sequence Player 節點的 On Become Relevant
	UFUNCTION(BlueprintCallable, Category="AnimGraph|NodeFunctions",
		meta=(BlueprintThreadSafe))
	void MyOnBecomeRelevant(const FAnimUpdateContext& Context, const FAnimNodeReference& Node);
	
	
	UFUNCTION(BlueprintCallable, Category="AnimGraph|NodeFunctions",
		meta=(BlueprintThreadSafe))
	UAnimSequence* GetSomeSequence(const ECardinalDirections InDirection);
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<AGZCharacterBase> Character;
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterMovementComponent> CMC;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float Speed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	FVector Direction; //pawn facing direction
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsCrouch;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bIsStrafing;

private:
	void InitializeCharacter();
};

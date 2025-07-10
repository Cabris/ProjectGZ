#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "JumpAnims.generated.h"

USTRUCT(BlueprintType)
struct FJumpAnims
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Animation")
	TObjectPtr<UAnimSequence> JumpStartAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Animation")
	TObjectPtr<UAnimSequence> JumpApexAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Animation")
	TObjectPtr<UAnimSequence> JumpLoopAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Animation")
	TObjectPtr<UAnimSequence> JumpRecoveryAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Animation")
	TObjectPtr<UAnimSequence> FallLandAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump Animation")
	TObjectPtr<UAnimSequence> FallLoopAnim;
};
#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AnimationStructs.generated.h"

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

USTRUCT(BlueprintType)
struct FTurnAnims
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequence> TurnRight90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequence> TurnRight180Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequence> TurnLeft90Anim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim Sequences")
	TObjectPtr<UAnimSequence> TurnLeft180Anim;
};

UENUM(BlueprintType)
enum class ETurnPoses :uint8
{
	NoTurn UMETA(DisplayName="No Turn"),
	TurnCW UMETA(DisplayName="Turn Clockwise"),
	TurnCCW UMETA(DisplayName="Turn Counterclockwise"),
};

UENUM(BlueprintType)
enum class  ERootBoneMode :uint8
{
	Accumulate UMETA(DisplayName="Accumulate"),
	BlendOut UMETA(DisplayName="BlendOut"),
	HoldOn UMETA(DisplayName="HoldOn")
};
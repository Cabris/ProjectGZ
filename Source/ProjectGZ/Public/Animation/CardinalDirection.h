#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CardinalDirection.generated.h"

USTRUCT(BlueprintType)
struct FCardinalDirectionAnims
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DirectionAnims")
	TObjectPtr<UAnimSequence> ForwardAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DirectionAnims")
	TObjectPtr<UAnimSequence> BackwardAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DirectionAnims")
	TObjectPtr<UAnimSequence> LeftAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DirectionAnims")
	TObjectPtr<UAnimSequence> RightAnim;
};

UENUM(BlueprintType)
enum class ECardinalDirections :uint8
{
	Forward UMETA(DisplayName="Forward"),
	Backward UMETA(DisplayName="Backward"),
	Left UMETA(DisplayName="Left"),
	Right UMETA(DisplayName="Right")
};

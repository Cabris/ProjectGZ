#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GZAnimationLayerSet.generated.h"

USTRUCT(BlueprintType)
struct FGZAnimationLayerEntry
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category="AnimationLayer")
	TSubclassOf<UAnimInstance> AnimationLayerClass;
	UPROPERTY(EditDefaultsOnly, Category="AnimationLayer")
	FGameplayTag AnimationLayerTag;
};

UCLASS()
class PROJECTGZ_API UGZAnimationLayerSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="AnimationLayerSet")
	TSubclassOf<UAnimInstance> GetAnimLayer(FGameplayTag Tag)
	{
		FGZAnimationLayerEntry* AnimLayer = AnimLayerSet.FindByPredicate([Tag](FGZAnimationLayerEntry& Entry)
		{
			return Entry.AnimationLayerTag == Tag;
		});
		if (AnimLayer)
		{
			return AnimLayer->AnimationLayerClass;
		}
		return DefaultAnimLayerClass;
	}

	UPROPERTY(EditDefaultsOnly, meta=(TitleProperty="AnimationLayerTag"))
	TArray<FGZAnimationLayerEntry> AnimLayerSet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UAnimInstance> DefaultAnimLayerClass;
};

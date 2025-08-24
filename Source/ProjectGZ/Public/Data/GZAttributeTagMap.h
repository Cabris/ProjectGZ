#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Fonts/UnicodeBlockRange.h"
#include "GZAttributeTagMap.generated.h"


USTRUCT(BlueprintType)
struct FAttributeTagMapItem
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	FGameplayAttribute Attribute;
	UPROPERTY(EditDefaultsOnly, Category="Attributes")
	FGameplayTag Tag;
};

UCLASS()
class PROJECTGZ_API UGZAttributeTagMap : public UDataAsset
{
	GENERATED_BODY()

public:
	bool HasTag(const FGameplayTag& Tag) const
	{
		return AttributeTagMap.ContainsByPredicate([Tag](const FAttributeTagMapItem& Item)
		{
			return Item.Tag == Tag;
		});
	}

	UFUNCTION(BlueprintPure, Category="Attributes")
	FGameplayAttribute GetAttributeByTag(const FGameplayTag& Tag) const
	{
		const FAttributeTagMapItem* FoundItem = AttributeTagMap.FindByPredicate([Tag](const FAttributeTagMapItem& Item)
		{
			return Item.Tag == Tag;
		});
		if (FoundItem)
			return FoundItem->Attribute;
		return FGameplayAttribute();
	}

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAttributeTagMapItem> AttributeTagMap;
};

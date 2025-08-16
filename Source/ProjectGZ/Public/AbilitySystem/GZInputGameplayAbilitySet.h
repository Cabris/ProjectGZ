#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Ability/GZGameplayAbility.h"
#include "Engine/DataAsset.h"
#include "GZInputGameplayAbilitySet.generated.h"

struct FGameplayAbilityBindInfo;

USTRUCT(BlueprintType)
struct FInputAbilityEntry
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGZGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;
};

UCLASS(Blueprintable,BlueprintType)
class PROJECTGZ_API UGZInputGameplayAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	const TArray<FInputAbilityEntry>& GetInputAbilityList() const
	{
		return Abilities;
	}

protected:
	UPROPERTY(EditAnywhere, Category = AbilitySet, meta=(TitleProperty="InputTag"))
	TArray<FInputAbilityEntry> Abilities;
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GZGameplayCueCatalog.generated.h"
USTRUCT(BlueprintType)
struct FGZCueEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly) FGameplayTag CueTag;
	UPROPERTY(EditDefaultsOnly) TSoftClassPtr<UObject> NotifyClass; // Burst/Static/Actor
};

UCLASS()
class PROJECTGZ_API UGZGameplayCueCatalog : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere) TArray<FGZCueEntry> Entries;
};

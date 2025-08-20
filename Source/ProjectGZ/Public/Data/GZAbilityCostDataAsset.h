#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "GZAbilityCostDataAsset.generated.h"

/** 一筆「由 SetByCaller 驅動的成本描述」 */
USTRUCT(BlueprintType)
struct PROJECTGZ_API FSetByCallerCostRow
{
	GENERATED_BODY()

	// 要修改的屬性（例如 Mana/Stamina/Ammo）
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttribute Attribute;

	// 這筆成本的 SetByCaller Key（GA 建 Spec 時會用同一個 Key 寫入數值）
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SetByCallerKey;

	// 修改方式（預設 Additive；消耗時 GA 傳負值）
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EGameplayModOp::Type> Op = EGameplayModOp::Additive;

	// 是否啟用這一筆（方便開關，不想刪掉）
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnabled = true;
};


UCLASS()
class PROJECTGZ_API UGZAbilityCostDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	const TArray<FSetByCallerCostRow>& GetAbilityCosts()
	{
		return AbilityCosts;
	}
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FSetByCallerCostRow> AbilityCosts;
};

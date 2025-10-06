#pragma once
#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "UObject/Object.h"
#include "GZCustomCost.generated.h"


/**
 * - DefaultToInstanced: 確保每個實例都是獨立的副本，避免共享狀態
 * - EditInlineNew: 允許在編輯器中直接創建和編輯子類實例
 * - Abstract: 防止直接實例化，強制通過繼承使用
 * 
 * 使用時需在 UPROPERTY 中添加 Instanced 修飾符：
 * UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite)
 * TObjectPtr<UGZCustomCost> Component;
 */

/**
 * A Custom Cost Object instanced in Editor properties window. 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class PROJECTGZ_API UGZCustomCost : public UObject
{
	GENERATED_BODY()

public:
	virtual bool CheckCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       OUT FGameplayTagContainer* OptionalRelevantTags) const PURE_VIRTUAL(UGZCustomCost::CheckCost, return true;);

	virtual void ApplyCost(const UGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
	                       const FGameplayAbilityActorInfo* ActorInfo,
	                       const FGameplayAbilityActivationInfo ActivationInfo) const PURE_VIRTUAL(UGZCustomCost::ApplyCost,);

protected:
private:
};

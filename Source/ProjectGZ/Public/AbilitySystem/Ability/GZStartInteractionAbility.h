#pragma once

#include "CoreMinimal.h"
#include "GZGameplayAbility.h"
#include "AbilitySystem/GZInteractionDetector.h"
#include "GZStartInteractionAbility.generated.h"

UCLASS()
class PROJECTGZ_API UGZStartInteractionAbility : public UGZGameplayAbility
{
	GENERATED_BODY()

public:
	UGZStartInteractionAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	// ===== 可調參數 =====
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	FGZInteractionDetectorConfig DetectorConfig;
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TSubclassOf<UGZInteractionDetector> DetectorClass;
private:
	// 檢測器實例
	UPROPERTY()
	TObjectPtr<UGZInteractionDetector> InteractionDetector;
	
	// 處理焦點變更回調
	UFUNCTION()
	void OnFocusChanged(AActor* NewFocus, AActor* OldFocus);
	bool AttemptInteraction_Internal();
};

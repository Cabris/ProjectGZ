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
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	// ===== 可調參數 =====
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	FGZInteractionDetectorConfig DetectorConfig;
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TSubclassOf<UGZInteractionDetector> DetectorClass;

private:
	bool AttemptInteraction_Internal(AActor* Target) const;
	void ClientSendTargetDataToServer(AActor* Target);
	void OnReceivedTargetDataFromClient(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag);
	bool ServerValidateTargetData(AActor* TargetFromClient) const;
	// 處理焦點變更回調, For Client UI Update
	UFUNCTION()
	void OnFocusChanged(AActor* NewFocus, AActor* OldFocus) ;
	// 檢測器實例
	UPROPERTY()
	TObjectPtr<UGZInteractionDetector> InteractionDetector;
	FDelegateHandle TargetDataSetDelegateHandle;
};

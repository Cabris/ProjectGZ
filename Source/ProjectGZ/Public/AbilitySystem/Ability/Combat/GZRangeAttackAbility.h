#pragma once
#include "CoreMinimal.h"
#include "GZWeaponAbility.h"
#include "Equipment/GZWeaponInstance.h"
#include "GZRangeAttackAbility.generated.h"

class UGZWeaponInstance;
//base class for ranged weapon fire ability: pistol, rifle, row.
UCLASS()
class PROJECTGZ_API UGZRangeAttackAbility : public UGZWeaponAbility
{
	GENERATED_BODY()

public:
	UGZRangeAttackAbility();
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
protected:
	bool CalculateFireResult(const FFireParams& FireParams, OUT FFireResult& FireResult);

	//logic for single shot
	UFUNCTION(BlueprintNativeEvent, Category="Ability|Combat")
	bool DoOneFire();

	UPROPERTY(Transient, BlueprintReadOnly, Category="Ability|Combat")
	TObjectPtr<UAnimMontage> FireMontage;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Ability|Combat")
	TObjectPtr<UAnimMontage> DryFireMontage;

private:
	//Loop body for repeat fire
	UFUNCTION()
	void TryDoFire();
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	void StartAutoFire();
	void StopAutoFire();
	void HandleDamage(FFireResult& Result);

	FTimerHandle FireTimerHandle;
	int32 CurrentFireIndex;
	float TimeSinceLastFire;
	//Runs on Client
	void ClientSendHitResultToServer(const FHitResult& HitResult);
	//Runs on Server
	void OnReceivedTargetDataFromClient(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag);
	FDelegateHandle TargetDataSetDelegateHandle;
};

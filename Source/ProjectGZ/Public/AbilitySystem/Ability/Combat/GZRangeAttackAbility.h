#pragma once
#include "CoreMinimal.h"
#include "GZWeaponAbility.h"
#include "Equipment/GZWeaponInstance.h"
#include "GZRangeAttackAbility.generated.h"

class UGZGameEffectCooldown;
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
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayClientFireFX(const FHitResult& HitResult);
	UFUNCTION(BlueprintImplementableEvent)
	void PlayClientFailFX();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayAuthorityFireFX(const FHitResult& HitResult);
	bool CalculateFireResult(const FFireParams& FireParams, OUT FFireResult& FireResult) const;
	//logic for single fire task
	bool DoFireInternal(OUT FFireResult& Result);

private:
	//check fire valid and do fire task
	UFUNCTION()
	void TryDoFire();
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	void InitializeCachedData();
	void StartAutoFire();
	void StopAutoFire();
	void HandleDamage(const FHitResult& HitResult);
	bool ServerValidateHitResult(const FHitResult& HitResult);

	FTimerHandle FireTimerHandle;
	int32 CurrentFireIndex;
	//Runs on Client
	void ClientSendHitResultToServer(const FHitResult& HitResult);
	//Runs on Server
	void OnReceivedTargetDataFromClient(const FGameplayAbilityTargetDataHandle& GameplayAbilityTargetDataHandle, FGameplayTag GameplayTag);
	FDelegateHandle TargetDataSetDelegateHandle;
};

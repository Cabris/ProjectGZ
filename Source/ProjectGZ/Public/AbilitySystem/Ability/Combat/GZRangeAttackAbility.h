#pragma once
#include "CoreMinimal.h"
#include "GZCombatAbility.h"
#include "Equipment/GZWeaponInstance.h"
#include "GZRangeAttackAbility.generated.h"

class UGZWeaponInstance;
//base class for ranged weapon fire ability: pistol, rifle, row.
UCLASS()
class PROJECTGZ_API UGZRangeAttackAbility : public UGZCombatAbility
{
	GENERATED_BODY()

public:
	UGZRangeAttackAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	//Loop body for repeat fire
	UFUNCTION(BlueprintNativeEvent, Category="Ability|Combat")
	void FireLoopBody();
	//logic for single shot
	UFUNCTION(BlueprintNativeEvent, Category="Ability|Combat")
	bool DoOneFire();

	UPROPERTY(Transient,BlueprintReadOnly, Category="Ability|Combat")
	TObjectPtr<UGZWeaponInstance> CachedWeaponInstance;
	UPROPERTY(Transient, BlueprintReadOnly, Category="Ability|Combat")
	FWeaponConfig CachedWeaponConfig;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="Ability|Combat")
	FAttackFilter AttackFilter;
	
private:
	void StartFireTimer();
	void StopFireTimer();
	FTimerHandle FireTimerHandle;
	UGZWeaponInstance* GetWeaponInstance() const;
	int32 CurrentFireIndex;
};

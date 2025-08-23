#include "AbilitySystem/Ability/Combat/GZRangeAttackAbility.h"

#include "GZFunctionLibrary.h"
#include "Character/GZCharacterBase.h"
#include "Equipment/GZWeaponInstance.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectGZ/ProjectGZ.h"

UGZRangeAttackAbility::UGZRangeAttackAbility()
{
	ActivationPolicy = EAbilityActivationPolicy::WhileInputActive;
}

void UGZRangeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AGZCharacterBase* Character = GetCharacter();
	FVector ViewOrigin;
	FVector ViewDirection;
	auto WeaponInst = GetWeaponInstance();
	if (!IsValid(WeaponInst))
	{
		Debug::Print(TEXT("ActivateAbility Fail: WeaponInstance is Null"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	CachedWeaponInstance = WeaponInst;
	CachedWeaponConfig = WeaponInst->GetConfig();

	//Timer
	StartFireTimer();

	if (UGZFunctionLibrary::GZHelper_GetViewPointFromActor(Character, ViewOrigin, ViewDirection))
	{
		FFireParams Params;
		Params.Origin = ViewOrigin;
		Params.Direction = ViewDirection;
		Params.FireIndex = 0;
		Params.Speed = 5000;
		Params.Filter = AttackFilter;
		FFireResult Result = WeaponInst->CalculateFireResult(Params);
	}
	else
	{
		Debug::Print(TEXT("UGZRangeAttackAbility::ActivateAbility: GZHelper_GetViewPointFromActor Failed!"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
}

void UGZRangeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	StopFireTimer();
}

UGZWeaponInstance* UGZRangeAttackAbility::GetWeaponInstance() const
{
	auto Equipment = GetEquipmentInstance();
	if (IsValid(Equipment))
		return Cast<UGZWeaponInstance>(Equipment);
	return nullptr;
}

void UGZRangeAttackAbility::StartFireTimer()
{
	if (FireTimerHandle.IsValid())
		StopFireTimer();
	float FireRate = CachedWeaponConfig.FireInterval > 0 ? (1.0f / CachedWeaponConfig.FireInterval ): 100;
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ThisClass::FireBody, FireRate, true);
}

void UGZRangeAttackAbility::StopFireTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UGZRangeAttackAbility::FireBody()
{
	
}

bool UGZRangeAttackAbility::DoOneFire()
{
	return true;
}

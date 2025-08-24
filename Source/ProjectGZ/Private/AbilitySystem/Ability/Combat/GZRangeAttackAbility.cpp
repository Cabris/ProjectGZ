#include "AbilitySystem/Ability/Combat/GZRangeAttackAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GZFunctionLibrary.h"
#include "Character/GZCharacterBase.h"
#include "Equipment/GZWeaponInstance.h"
#include "ProjectGZ/ProjectGZ.h"
#include "AbilitySystem/Effect/GZDamageEffect.h"


UGZRangeAttackAbility::UGZRangeAttackAbility()
{
	ActivationPolicy = EAbilityActivationPolicy::WhileInputActive;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	CurrentFireIndex = 0;
}

void UGZRangeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	CachedWeaponInstance = GetWeaponInstance();
	if (!IsValid(CachedWeaponInstance))
	{
		Debug::Print(TEXT("ActivateAbility Fail: WeaponInstance is Null"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	CachedWeaponConfig = CachedWeaponInstance->GetConfig();

	StartFireTimer();
}

void UGZRangeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	StopFireTimer();
}

void UGZRangeAttackAbility::FireLoopBody_Implementation()
{
	if (DoOneFire())
		CurrentFireIndex += 1;
	else
	{
		StopFireTimer();
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

bool UGZRangeAttackAbility::DoOneFire_Implementation()
{
	AGZCharacterBase* Character = GetCharacter();
	if (!IsValid(Character) || !IsValid(CachedWeaponInstance))
	{
		Debug::Print(TEXT("UGZRangeAttackAbility::ActivateAbility: GZHelper_GetViewPointFromActor Failed!"));
		return false;
	}

	bool bCheckCost = CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	if (!bCheckCost)
	{
		Debug::Print(TEXT("UGZRangeAttackAbility::ActivateAbility: CommitAbilityCost Failed!"));
		return false;
	}

	FVector ViewOrigin;
	FVector ViewDirection;
	if (!UGZFunctionLibrary::GZHelper_GetViewPointFromActor(Character, ViewOrigin, ViewDirection))
		return false;

	FFireParams Params;
	Params.Origin = ViewOrigin;
	Params.Direction = ViewDirection;
	Params.FireIndex = CurrentFireIndex;
	Params.InitialSpeed = CachedWeaponConfig.InitialSpeed;
	Params.MaxRange = CachedWeaponConfig.MaxRangeMeters * 100;
	Params.Filter = AttackFilter;
	Params.bDrawDebug = true;
	FFireResult Result = CachedWeaponInstance->CalculateFireResult(Params);

	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !IsValid(ASC->GetOwner()))return false;

	if (!Result.bIsHit)return true;

	AActor* HitActor = Result.Hit.GetActor();
	if (!IsValid(HitActor))return true;
	auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC) return true;

	//GE
	// 建立 GameplayEffect Context
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	// Instigator 與 Controller
	ContextHandle.AddInstigator(Character, Character->GetController());
	// 來源武器/裝備（讓 GE 知道是由哪把槍造成的）
	ContextHandle.AddSourceObject(CachedWeaponInstance);
	// 命中資訊（爆頭/命中骨頭、命中點、法線）
	ContextHandle.AddHitResult(Result.Hit);

	// 建立 GE 規格（Spec）
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, AbilityLevel, ContextHandle);
	if (!SpecHandle.IsValid())return false;

	float Damage = CachedWeaponConfig.BaseDamage;
	auto DamageGECDO = DamageEffectClass.GetDefaultObject();
	if (!IsValid(DamageGECDO))return false;

	auto AppliedDamagePropertyTag = DamageGECDO->GetAppliedDamagePropertyTag();
	if (AppliedDamagePropertyTag.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(AppliedDamagePropertyTag, Damage);
	}

	// 正式套用 GE 到目標
	const auto ActiveHandle = TargetASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	// 成功後可在此觸發 GameplayCue（若 GE 本身沒帶 Cue）
	// UAbilitySystemBlueprintLibrary::SendGameplayCue(...)

	bool bSuccessApply = ActiveHandle.WasSuccessfullyApplied();
	FString Msg = FString::Printf(TEXT("Apply DamageEffect: %d"), bSuccessApply);
	Debug::Print(Msg);
	return true;
}

void UGZRangeAttackAbility::StartFireTimer()
{
	if (FireTimerHandle.IsValid())
		StopFireTimer();
	float FireRate = CachedWeaponConfig.FireInterval > 0 ? (1.0f / CachedWeaponConfig.FireInterval) : 10;
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ThisClass::FireLoopBody, FireRate, true);
	CurrentFireIndex = 0;
}

void UGZRangeAttackAbility::StopFireTimer()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
	CurrentFireIndex = 0;
}

UGZWeaponInstance* UGZRangeAttackAbility::GetWeaponInstance() const
{
	auto Equipment = GetEquipmentInstance();
	if (IsValid(Equipment))
		return Cast<UGZWeaponInstance>(Equipment);
	return nullptr;
}

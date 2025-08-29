#include "AbilitySystem/Ability/Combat/GZRangeAttackAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GZFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/GZCharacterBase.h"
#include "Equipment/GZWeaponInstance.h"
#include "ProjectGZ/ProjectGZ.h"
#include "AbilitySystem/Effect/GZDamageEffect.h"


UGZRangeAttackAbility::UGZRangeAttackAbility()
{
	ActivationPolicy = EAbilityActivationPolicy::WhileInputActive;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	CurrentFireIndex = 0;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	TimeSinceLastFire = 0;
	FireMontage = nullptr;
	DryFireMontage = nullptr;
}

void UGZRangeAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	CachedWeaponInstance = nullptr;
	TimeSinceLastFire = 0;
	FireMontage = nullptr;
	DryFireMontage = nullptr;
	CurrentFireIndex = 0;
}

void UGZRangeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	auto ASC = GetAbilitySystemComponent();
	CachedWeaponInstance = GetWeaponInstance();
	if (!IsValid(CachedWeaponInstance))
	{
		Debug::Print(TEXT("ActivateAbility Fail: WeaponInstance is Null"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	CachedWeaponConfig = CachedWeaponInstance->GetConfig();

	if (!IsLocalControlled()) //On Server
	{
		//Bind delegate for receive TargetData from Client version UGZRangeAttackAbility
		FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
		FPredictionKey OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
		FAbilityTargetDataSetDelegate& Delegate = ASC->AbilityTargetDataSetDelegate(SpecHandle, OriginalPredictionKey);
		TargetDataSetDelegateHandle = Delegate.AddUObject(this, &ThisClass::OnReceivedTargetDataFromClient);
		//ensure that TargetData received before Bind delegate, return is CalledDelegate
		bool bCalledDelegate = ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, OriginalPredictionKey);
		if (!bCalledDelegate)
		{
			//設定伺服器端等待PlayerData資料的上傳
		}
	}

	//TODO: use async load
	FireMontage = CachedWeaponConfig.FireMontage.LoadSynchronous();
	DryFireMontage = CachedWeaponConfig.DryFireMontage.LoadSynchronous();

	if (CachedWeaponConfig.FireMode == EFireMode::Auto)
	{
		StartAutoFire();
		auto WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
		WaitReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
		WaitReleaseTask->ReadyForActivation();
	}
	else if (CachedWeaponConfig.FireMode == EFireMode::Semi)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		float TimeSpan = CurrentTime - TimeSinceLastFire;

		if (TimeSpan <= CachedWeaponConfig.FireInterval)
		{
			TryDoFire();
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
		else
		{
			Debug::Print(TEXT("UGZRangeAttackAbility::ActivateAbility: need to wait for FireInterval!"));
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		}
	}
}

void UGZRangeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                       const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	auto ASC = GetAbilitySystemComponent();
	if (TargetDataSetDelegateHandle.IsValid())
	{
		FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
		FPredictionKey OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
		FAbilityTargetDataSetDelegate& Delegate = ASC->AbilityTargetDataSetDelegate(SpecHandle, OriginalPredictionKey);
		Delegate.Remove(TargetDataSetDelegateHandle);
	}
}

void UGZRangeAttackAbility::OnInputReleased(float TimeHeld)
{
	StopAutoFire();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGZRangeAttackAbility::TryDoFire()
{
	bool bIsFired = DoOneFire();
	if (bIsFired)
	{
		CurrentFireIndex += 1;
	}
	else
	{
		StopAutoFire();
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

bool UGZRangeAttackAbility::DoOneFire_Implementation()
{
	//////////Fire Check
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
	//////////////

	//Play fire montage
	if (FireMontage)
	{
		UAbilityTask_PlayMontageAndWait* WaitMontage =
			UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName("WeaponFireMontage"), FireMontage, 1);
		//WaitMontage->OnCompleted.AddDynamic(this, &UGZBowAttackTestAbility::OnMontageCompleted);
		WaitMontage->ReadyForActivation();
	}

	//Fire Logic
	FFireParams Params;
	Params.Origin = ViewOrigin;
	Params.Direction = ViewDirection;
	Params.FireIndex = CurrentFireIndex;
	Params.InitialSpeed = CachedWeaponConfig.InitialSpeed;
	Params.MaxRange = CachedWeaponConfig.MaxRangeMeters * 100;
	Params.Filter = AttackFilter;
	Params.bDrawDebug = true;

	FFireResult Result;
	bool bIsResultValid = CalculateFireResult(Params, Result);
	if (bIsResultValid)
	{
		FHitResult& Hit = Result.Hit;
		if (IsLocalControlled())
		{
			ClientSendHitResultToServer(Hit);
		}

		//set last fire time
		TimeSinceLastFire = GetWorld()->GetTimeSeconds();
		HandleDamage(Result);
	}


#ifdef  WITH_EDITOR
	if (Params.bDrawDebug)
	{
		DrawDebugLine(GetWorld(), Params.Origin, Result.EndLocation, FColor::MakeRandomColor(),
		              false, 0.5f, 0, 1.0f);
	}
#endif

	return true;
}

bool UGZRangeAttackAbility::CalculateFireResult(const FFireParams& Params, OUT FFireResult& FireResult)
{
	if (!ensure(Params.Direction.IsNormalized()))
	{
		FireResult.bIsValid = false;
		return false;
	}

	const FVector Start = Params.Origin;
	const FVector End = Start + Params.Direction * Params.MaxRange;

	FCollisionQueryParams QueryParams;
	UGZWeaponInstance::BuildCollisionParams(Params.Filter, QueryParams);

	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, Params.Filter.TraceChannel, QueryParams);

	if (bHit)
	{
		FireResult.bIsValid = true;
		FireResult.Hit = Hit;
		FireResult.bIsHit = true;
		FireResult.EndLocation = Hit.ImpactPoint;
	}
	else
	{
		FireResult.bIsValid = true;
		FireResult.EndLocation = End;
	}
	return true;
}

void UGZRangeAttackAbility::HandleDamage(FFireResult& Result)
{
	AGZCharacterBase* Character = GetCharacter();

	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !IsValid(ASC->GetOwner()))return;
	if (!Result.bIsHit)return;
	AActor* HitActor = Result.Hit.GetActor();
	if (!IsValid(HitActor))return;
	auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC) return;

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
	if (!SpecHandle.IsValid())return;

	float Damage = CachedWeaponConfig.BaseDamage;
	auto DamageGECDO = DamageEffectClass.GetDefaultObject();
	if (!IsValid(DamageGECDO))return;

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
}

void UGZRangeAttackAbility::ClientSendHitResultToServer(const FHitResult& HitResult)
{
	auto ASC = GetAbilitySystemComponent();
	FScopedPredictionWindow PredictionWindow(ASC, true);
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitResult;
	FGameplayAbilityTargetDataHandle TargetDataHandle(Data);
	auto OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
	//Send Target Data to Server
	ASC->ServerSetReplicatedTargetData(
		GetCurrentAbilitySpecHandle(),
		OriginalPredictionKey,
		TargetDataHandle,
		FGameplayTag(),
		ASC->ScopedPredictionKey);
}

void UGZRangeAttackAbility::OnReceivedTargetDataFromClient(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag GameplayTag)
{
	auto ASC = GetAbilitySystemComponent();
	FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
	FPredictionKey OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
	ASC->ConsumeClientReplicatedTargetData(SpecHandle, OriginalPredictionKey);

	for (int Idx = 0; Idx < TargetDataHandle.Num(); Idx++)
	{
		const FGameplayAbilityTargetData* Data = TargetDataHandle.Get(Idx);
		if (Data && Data->HasHitResult())
		{
			const FHitResult* HitResult = Data->GetHitResult();
			FString Msg = FString::Printf(TEXT("OnReceivedTargetDataFromClient: HitResult->ImpactPoint: %s"), *HitResult->ImpactPoint.ToString());
			Debug::Print(Msg);
			//TODO: do server side check stuff...
		}
	}
}

void UGZRangeAttackAbility::StartAutoFire()
{
	if (FireTimerHandle.IsValid())
		StopAutoFire();
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ThisClass::TryDoFire, CachedWeaponConfig.FireInterval, true);
}

void UGZRangeAttackAbility::StopAutoFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

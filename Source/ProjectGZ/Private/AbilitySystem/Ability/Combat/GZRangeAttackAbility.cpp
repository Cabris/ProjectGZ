#include "AbilitySystem/Ability/Combat/GZRangeAttackAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GZFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "AbilitySystem/Ability/Combat/GZAbilityTargetData_SingleTargetHit.h"
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
}

void UGZRangeAttackAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	CachedWeaponInstance = nullptr;
	TimeSinceLastFire = 0;
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
		ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, OriginalPredictionKey);
	}

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
	if (FireTimerHandle.IsValid())
		StopAutoFire();
}

void UGZRangeAttackAbility::OnInputReleased(float TimeHeld)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGZRangeAttackAbility::TryDoFire()
{
	FFireResult FireResult;
	if (!DoFireInternal(FireResult))
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

bool UGZRangeAttackAbility::DoFireInternal(OUT FFireResult& Result)
{
	if (IsLocalControlled())
	{
		//開始預測區域
		FScopedPredictionWindow PredictionWindow(GetAbilitySystemComponent(), true);
	}

	//Fire Check
	AGZCharacterBase* Character = GetCharacter();
	if (!IsValid(Character) || !IsValid(CachedWeaponInstance))
	{
		Debug::Print(TEXT("UGZRangeAttackAbility::ActivateAbility: GZHelper_GetViewPointFromActor Failed!"));
		return false;
	}

	FVector ViewOrigin;
	FVector ViewDirection;
	if (!UGZFunctionLibrary::GZHelper_GetViewPointFromActor(Character, ViewOrigin, ViewDirection))
		return false;
	//End Fire Check

	//Fire Logic
	FFireParams Params;
	Params.Origin = ViewOrigin;
	Params.Direction = ViewDirection;
	Params.FireIndex = CurrentFireIndex;
	Params.InitialSpeed = CachedWeaponConfig.InitialSpeed;
	Params.MaxRange = CachedWeaponConfig.MaxRangeMeters * 100;
	Params.Filter = AttackFilter;
	Params.bDrawDebug = true;

	//有成功發射
	if (CalculateFireResult(Params, Result))
	{
		FHitResult& Hit = Result.Hit;
		//if on client, do stuff before server check
		if (IsLocalControlled())
		{
			CurrentFireIndex += 1;
			//Do Client Fire Prediction
			PlayClientFireFX(Hit);
			//FGameplayTagContainer FailReasonTags;
			CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
			//set last fire time
			TimeSinceLastFire = GetWorld()->GetTimeSeconds();
			ClientSendHitResultToServer(Hit);
		}
		return true;
	}
	return false;
}

void UGZRangeAttackAbility::ClientSendHitResultToServer(const FHitResult& HitResult)
{
	FGZAbilityTargetData_SingleTargetHit* Data = new FGZAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitResult;
	Data->HitIndex = CurrentFireIndex;
	//TODO: ccheck ClientTimestamp
	Data->ClientTimestamp = GetWorld()->GetTimeSeconds();
	FGameplayAbilityTargetDataHandle TargetDataHandle(Data);
	auto OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
	auto ASC = GetAbilitySystemComponent();

	//Send Target Data to Server
	//ASC->CallServerSetReplicatedTargetData();
	FGameplayTag ApplicationTag;
	ASC->ServerSetReplicatedTargetData(
		GetCurrentAbilitySpecHandle(),
		OriginalPredictionKey,
		TargetDataHandle,
		ApplicationTag,
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
		const FGameplayAbilityTargetData* BaseData = TargetDataHandle.Get(Idx);
		//guarantee the BaseData is the type we want
		if (BaseData && BaseData->HasHitResult() && BaseData->GetScriptStruct()->IsChildOf(FGZAbilityTargetData_SingleTargetHit::StaticStruct()))
		{
			const FGZAbilityTargetData_SingleTargetHit* Data = static_cast<const FGZAbilityTargetData_SingleTargetHit*>(BaseData);
			const FHitResult* HitResult = Data->GetHitResult();
			const int HitIndex = Data->HitIndex;
			FString Msg = FString::Printf(TEXT("OnReceivedTargetDataFromClient: HitResult->ImpactPoint: %s, HitIndex: %d"),
			                              *HitResult->ImpactPoint.ToString(), HitIndex);
			Debug::Print(Msg);
			//TODO: do server side check stuff...
			bool bServerValidate = ServerValidateHitResult(*HitResult);
			if (bServerValidate)
			{
				HandleDamage(*HitResult);
				PlayAuthorityFireFX(*HitResult);
			}
			else
			{
				Debug::Print(TEXT("ServerValidateHitResult Failed"));
				CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			}
		}
	}
}

bool UGZRangeAttackAbility::ServerValidateHitResult(const FHitResult& ClientHit)
{
	FFireResult ServerResult;
	bool bServerValid = DoFireInternal(ServerResult);
	if (!bServerValid) return false;

	// 驗證客戶端結果是否合理（考慮網路延遲容差）
	if (ServerResult.bIsHit && ClientHit.bBlockingHit)
	{
		float DistanceDiff = FVector::Dist(ClientHit.ImpactPoint, ServerResult.Hit.ImpactPoint);
		const float MaxAllowedDifference = 50.0f; // 5cm 容差
		return DistanceDiff <= MaxAllowedDifference;
	}

	return ServerResult.bIsHit == ClientHit.bBlockingHit;
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


bool UGZRangeAttackAbility::CalculateFireResult(const FFireParams& Params, OUT FFireResult& FireResult) const
{
	if (!Params.Direction.IsNormalized())
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

#ifdef  WITH_EDITOR
	if (Params.bDrawDebug)
	{
		DrawDebugLine(GetWorld(), Params.Origin, FireResult.EndLocation, FColor::MakeRandomColor(),
		              false, 0.5f, 0, 1.0f);
	}
#endif
	return true;
}


void UGZRangeAttackAbility::HandleDamage(const FHitResult& Result)
{
	UGZAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !IsValid(ASC->GetOwner()))return;
	AActor* HitActor = Result.GetActor();
	if (!IsValid(HitActor))return;
	auto TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC) return;

	//GE
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	AGZCharacterBase* Character = GetCharacter();
	ContextHandle.AddInstigator(Character, Character->GetController());
	// 來源武器/裝備（讓 GE 知道是由哪把槍造成的）
	ContextHandle.AddSourceObject(CachedWeaponInstance);
	// 命中資訊（爆頭/命中骨頭、命中點、法線）
	ContextHandle.AddHitResult(Result);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, AbilityLevel, ContextHandle);
	if (!SpecHandle.IsValid())return;

	float Damage = CachedWeaponConfig.BaseDamage;
	auto DamageGECDO = DamageEffectClass.GetDefaultObject();
	if (!IsValid(DamageGECDO))return;

	auto AppliedDamagePropertyTag = DamageGECDO->GetAppliedDamagePropertyTag();
	if (!AppliedDamagePropertyTag.IsValid())
	{
		Debug::Print(TEXT("Failed to GetAppliedDamagePropertyTag"));
		return;
	}
	SpecHandle.Data->SetSetByCallerMagnitude(AppliedDamagePropertyTag, Damage);
	// 正式套用 GE 到目標
	const auto ActiveHandle = TargetASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	// 成功後可在此觸發 GameplayCue（若 GE 本身沒帶 Cue）
	// UAbilitySystemBlueprintLibrary::SendGameplayCue(...)
	bool bSuccessApply = ActiveHandle.WasSuccessfullyApplied();
	FString Msg = FString::Printf(TEXT("Apply DamageEffect: %d"), bSuccessApply);
	Debug::Print(Msg);
}

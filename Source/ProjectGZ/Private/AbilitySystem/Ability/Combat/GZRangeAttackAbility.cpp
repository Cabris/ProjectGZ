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
#include "AbilitySystem/Effect/GZGameEffectCooldown.h"


UGZRangeAttackAbility::UGZRangeAttackAbility()
{
	ActivationPolicy = EAbilityActivationPolicy::WhileInputActive;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	CurrentFireIndex = 0;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	CachedWeaponInstance = nullptr;
	CooldownTags.AddTag(GZGameplayTags::Cooldown_Generic);
}

void UGZRangeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                            const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	auto ASC = GetAbilitySystemComponent();
	if (!IsValid(CachedWeaponInstance))
	{
		Debug::Print(TEXT("ActivateAbility Fail: WeaponInstance is Null"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	//On Dedicated Server or Listen Server, but not On Client
	if (IsNetAuthority() && !IsLocalControlled() || IsListenServer())
	{
		//Bind delegate for receive TargetData from Client version UGZRangeAttackAbility
		FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
		FPredictionKey OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
		FAbilityTargetDataSetDelegate& Delegate = ASC->AbilityTargetDataSetDelegate(SpecHandle, OriginalPredictionKey);
		TargetDataSetDelegateHandle = Delegate.AddUObject(this, &ThisClass::OnReceivedTargetDataFromClient);
		//ensure that TargetData received before Bind delegate, return is CalledDelegate
		ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, OriginalPredictionKey);
	}

	auto WaitReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, false);
	WaitReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
	WaitReleaseTask->ReadyForActivation();

	if (CachedWeaponConfig.FireMode == EFireMode::Auto)
	{
		StartAutoFire();
	}
	else if (CachedWeaponConfig.FireMode == EFireMode::Semi)
	{
		TryDoFire();
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
	//DEBUG_PRINTF(TEXT("UGZRangeAttackAbility::EndAbility: IsLocalControlled= %d"), IsLocalControlled());
}

void UGZRangeAttackAbility::OnInputReleased(float TimeHeld)
{
	// FString Msg = FString::Printf(TEXT("UGZRangeAttackAbility::OnInputReleased: IsLocalControlled= %d"), IsLocalControlled());
	// Debug::Print(Msg);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGZRangeAttackAbility::TryDoFire()
{
	FScopedPredictionWindow PredictionWindow(GetAbilitySystemComponent(), true); //開始預測區域
	FGameplayTagContainer CostFailReasonTags;
	FGameplayTagContainer CooldownFailReasonTags;
	bool bCostSuccess = CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, &CostFailReasonTags);
	bool bCooldown = CommitAbilityCooldown(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, &CooldownFailReasonTags);

	bool bIsLocalControlled = IsLocalControlled();
	if (bIsLocalControlled)
	{
		FFireResult FireResult;
		if (bCostSuccess && DoFireInternal(FireResult))
		{
			FHitResult& Hit = FireResult.Hit;
			PlayFireFX(Hit);
			ClientSendHitResultToServer(Hit);
			FString Msg = FString::Printf(TEXT("UGZRangeAttackAbility::Client::Fired: CurrentFireIndex: %d"), CurrentFireIndex);
			Debug::Print(Msg);
			CurrentFireIndex += 1;
		}
		else
		{
			PlayFailFX();
			CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		}
	}
}

bool UGZRangeAttackAbility::DoFireInternal(OUT FFireResult& Result)
{
	FVector ViewOrigin;
	FVector ViewDirection;

	//Make Ray data
	AGZCharacterBase* Character = GetCharacter();
	if (!IsValid(Character) || !IsValid(CachedWeaponInstance))
	{
		Debug::Print(TEXT("UGZRangeAttackAbility::DoFireInternal: GetCharacter Failed!"));
		return false;
	}
	if (!UGZFunctionLibrary::GZHelper_GetViewPointFromActor(Character, ViewOrigin, ViewDirection))
	{
		Debug::Print(TEXT("UGZRangeAttackAbility::DoFireInternal: GZHelper_GetViewPointFromActor Failed!"));
		return false;
	}
	//Make Ray data

	//Fire Logic
	FFireParams Params;
	Params.Origin = ViewOrigin;
	Params.Direction = ViewDirection;
	Params.FireIndex = CurrentFireIndex;
	Params.InitialSpeed = CachedWeaponConfig.InitialSpeed;
	Params.MaxRange = CachedWeaponConfig.MaxRangeMeters * 100;
	Params.Filter = AttackFilter;
	Params.bDrawDebug = CachedWeaponConfig.bDrawDebug;

	bool bFireSuccess = CalculateFireResult(Params, Result);
	if (!bFireSuccess)
		return false;

	//有成功發射
	return true;
}

void UGZRangeAttackAbility::ClientSendHitResultToServer(const FHitResult& HitResult)
{
	FGZAbilityTargetData_SingleTargetHit* Data = new FGZAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitResult;
	Data->FireIndex = CurrentFireIndex;
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

void UGZRangeAttackAbility::OnReceivedTargetDataFromClient(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                           FGameplayTag GameplayTag)
{
	auto ASC = GetAbilitySystemComponent();
	FGameplayAbilitySpecHandle SpecHandle = GetCurrentAbilitySpecHandle();
	FPredictionKey OriginalPredictionKey = GetCurrentActivationInfo().GetActivationPredictionKey();
	ASC->ConsumeClientReplicatedTargetData(SpecHandle, OriginalPredictionKey);

	for (int Idx = 0; Idx < TargetDataHandle.Num(); Idx++)
	{
		const FGameplayAbilityTargetData* BaseData = TargetDataHandle.Get(Idx);
		//guarantee the BaseData is the type we want
		if (BaseData && BaseData->HasHitResult() && BaseData->GetScriptStruct()->IsChildOf(
			FGZAbilityTargetData_SingleTargetHit::StaticStruct()))
		{
			const FGZAbilityTargetData_SingleTargetHit* Data = static_cast<const FGZAbilityTargetData_SingleTargetHit*>(BaseData);
			const FHitResult* HitResult = Data->GetHitResult();
			const int FireIndex = Data->FireIndex;

			bool bServerValidate = ServerValidateHitResult(*HitResult);
			if (bServerValidate)
			{
				//server comfirm client
				HandleDamage(*HitResult);
				PlayFireFX(*HitResult);
				FString Msg = FString::Printf(TEXT("UGZRangeAttackAbility::Server::Fired: CurrentFireIndex: %d"), FireIndex);
				Debug::Print(Msg);
			}
			else
			{
				//server found client may cheat
				Debug::Print(TEXT("UGZRangeAttackAbility::ServerValidateHitResult Failed"));
				PlayFailFX();
				CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
			}
		}
	}
}

bool UGZRangeAttackAbility::ServerValidateHitResult(const FHitResult& ClientHit)
{
	FFireResult ServerResult;
	bool bServerValid = DoFireInternal(ServerResult);
	if (!bServerValid)
	{
		Debug::Print(TEXT("UGZRangeAttackAbility::ServerValidateHitResult bServerValid Failed"));
		return false;
	}

	// 驗證客戶端結果是否合理（考慮網路延遲誤差）
	if (ServerResult.bIsHit && ClientHit.bBlockingHit)
	{
		float DistanceDiff = FVector::Dist(ClientHit.ImpactPoint, ServerResult.Hit.ImpactPoint);
		const float MaxAllowedDifference = 50.0f; // 50cm 誤差範圍
		bool bDistanceCond = DistanceDiff <= MaxAllowedDifference;
		if (!bDistanceCond)
			Debug::Print(TEXT("UGZRangeAttackAbility::ServerValidateHitResult bDistanceCond Failed"));
		return bDistanceCond;
	}
	bool bIsSameHit = ServerResult.bIsHit == ClientHit.bBlockingHit;
	if (!bIsSameHit)
		Debug::Print(TEXT("UGZRangeAttackAbility::ServerValidateHitResult bIsSameHit Failed"));
	return bIsSameHit;
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
		Debug::Print(TEXT("UGZRangeAttackAbility::CalculateFireResult Failed, Direction not Normalized"));
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
	if (Params.bDrawDebug && HasAuthority(&CurrentActivationInfo))
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
